# mapaddr_example

The mapaddr program is used to match addresses in a stack trace
to function names in a map file. Everytime the firmware in the 
ARM processor performs a branch-link instruction, the current 
program counter (PC) is saved in the link register (LR). If the
called function intends to execute instructions that overwrite 
the link register (e.g., branch-link instructions), it pushes 
the current link register value to the stack during its prologue
and pops it in its epilogue. Even though, there is no base 
pointer that indicates the length of stack frames, it is 
possible to go through a stack dump and follow the pushed 
addresses to create a function call trace.

To create the call trace, the mapaddr_example is based on the
debugger_example to set a breakpoint in the firmware, where we
intend to dump the stack. In the following example, this would
be the dma_txfast function that sends an sk_buff either to the
D11 core, or the SDIO core:
```
dbg_set_breakpoint_for_addr_match(0, 0x1844B2); // dma_txfast
```

When the breakpoint hits, it first checks if the target of the
dma_txfast call is not the DMA of the SDIO controller and then
creates a new frame that contains a stack trace, similar to the
udp_to_android_example.
```
if (trace->r0 != 0x1eab98) {
  p = create_frame(trace->pc, trace->r0, trace->r1, trace->r2, 
    (void *) trace->sp, 
    BOTTOM_OF_STACK - (unsigned int) trace->sp);
  dngl_sendpkt(SDIO_INFO_ADDR, p, SDPCM_DATA_CHANNEL);
}
```

The created frames encapsulate the stack trace into UDPLite, 
IPv6 and Ethernet frames that can be received by Android user
space applications. One can either dump those frames into a file
using tcpdump, or get live traces by telling mapaddr to listen 
on an interface.

## How to run the example?

To compile the mapaddr tool, you need the libpcap sources placed
in the external directory in your NDK root. The mapaddr tool can
be compiled for Android (bcm-public/bootimg_src/src/mapaddr) or
for a Linux host (bcm-public/buildtools/mapaddr). The latter is
useful to analyze pcap dumps from tcpdump offline.

To run the example, first load the patched firmware and driver:
```
make reloadfirmware FWPATCH=mapaddr_example
```

Then, set up the wifi interface:
```
adb shell "su -c 'ifconfig wlan0 up'"
```

Then start mapaddr and tell it to listen on the wlan0 interface:
```
adb shell "su -c 'mapaddr -c wlan0'"
```

To generate some calls to dma_txfast, we scan for access points:
```
adb shell "su -c 'iwlist wlan0 scan'"
```

The output of the mapaddr tool should contain the following:
```
dma_txfast 001844b2 001e2e24 001d1424 00000001
0022 0000fffa: get_erom_ent (0000ff98+98) BL malloc_with_osh_wrapper
0046 00192814: wlc_send_q (001926b8+348) BL wlc_txfifo
0095 00001d10: nullsub_32 (00001bee+290) BL questionmark_cmd_setup_stuff
0222 00036aba: wlc_pspoll_timer_upd (00036960+346) BL wlc_prec_enq_head_wrapper
0262 00038e38: transmit_key_stuff (000389d0+1128) BL wlc_pspoll_timer_upd
0286 001977bc: wlc$_wlc_ioctl (0019551c+8864) BL transmit_key_stuff
0291 00001d10: nullsub_32 (00001bee+290) BL questionmark_cmd_setup_stuff
0415 00001d10: nullsub_32 (00001bee+290) BL questionmark_cmd_setup_stuff
0446 00074c68: def_745E0 (0007468e+1498) BL transmit_key_stuff
0502 001c8cd6: wlc_compute_rspec (001c82f4+2530) BLX r3
0534 001c9374: wlc_compute_rspec (001c82f4+4224) BL wlc_compute_rspec
0590 00027644: called_by_intr_handler_calls_wlc_dpc_caller (00027550+244) BLX r3
0598 0001649a: print_start_len_count (000163f8+162) BLX r3
0606 000164de: print_start_len_count (000163f8+230) BLX r3
0622 001819e4: hndrte_print_memuse (001817a4+576) BL print_start_len_count
0623 00001814: setup_console_cmd_callback (000017a4+112) BL aIHf
0630 00015680: indirectly_called_by_intr_handler (00015654+44) BLX r3
0646 00181aa4: external_interrupt_handler (00181a88+28) BLX r3
0738 000166c4: enable_interrupts_and_wait (000166b4+16) BL wait_for_interrupt
```

The output tells you that function in which the stack trace
was created based on the address passed as first argument to the
create frame function in the firmware. Then each line give you
(1) the position in the stack trace, (2) the value at this 
position, (3) the identified name of the function that includes
this address, (4) in parenthesis the base address of the 
function plus the offset to the value on the stack, (5) a 
decompiled instruction at the address on the stack. By default,
only those addresses on the stack are printed that follow a
branch instruction (to print all, pass the -n option to 
mapaddr). Even though looking for branch instructions already
filters out many false positives on the stack, not all entries
are valid. Also keep in mind that interrupts can trigger the 
generation of a new call graph path on the stack.