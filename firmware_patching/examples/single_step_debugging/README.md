# debugger_example

This example demonstrates how to use the hardware debug 
registers in the ARM Cortex-R4 core that runs the firmware in 
the BCM4339 WiFi chip. Using the patch, we can trigger prefetch
abort exceptions by hardware breakpoints and data abort 
exceptions by watchpoints. The exceptions are generated as the
debugging system is configured for monitor-mode debugging.
Alternatively, it is also possible to halt the execution and 
connect an external debugger, but this would disrupt the program
execution too much. After handling one of the generated 
exeptions, we can continue program execution. To continue at the
instruction that triggered the breakpoint, we first have to 
disable our breakpoint, otherwise we would trigger it again 
before executing the instruction. To reenable the breakpoint 
after executing the instruction, we set another breakpoint to 
trigger on an instruction mismatch and reset the original 
breakpoint when handling the exception generated on the 
instruction mismatch. This is similar to single step debugging.
To avoid triggering the instruction mismatch in the exception 
handler directly after setting the instruction mismatch 
breakpoint, we have to execute our exception handlers in a 
different processor mode than the regular firmware. Generally,
the firmware runs in system mode and after triggering prefetch
or data abort exceptions, the processor automatically switches
into abort mode. The original firmware directly switches back
to system mode. Our patch, however, needs to stay in abort mode 
and also allocate a stack for function calls in this mode.

## Function Explanation

The debugger patch consists of multiple functions, of which I
present the most important in the following:

### set_debug_registers:
This function is called at the beginning of the initialization
phase of the WiFi chip, shortly after it comes out of reset. It
first places the stack pointer of the abort mode to the end of
the regular stack that is generally not used during regular 
program execution. Then we unlock write access to the debug 
registers, initialize them to be disabled and activate monitor-
mode debugging. After that we can simply set breakpoints and
watchpoints.

### tr_pref_abort_hook:
### tr_data_abort_hook:
Both functions overwrite the assembler code that is executed 
directly after triggering an exception. Here, we avoid switching
to system mode and stay in abort mode. A prefetch abort 
exception is indicated by number 3 in register 0 and a data
about exception by number 4.

### handle_exceptions:
### choose_exception_handler:
We extended the existing handle_exceptions function by the
choose_exception_handler function and use it to evaluate the
exception number saved before in register 0. On a prefetch abort
exception, we call the handle_pref_abort_exception handler and 
on a data abort exception, we call the 
handle_data_abort_exception handler.

### fix_sp_lr:
At the beginning of both exception handlers 
handle_pref_abort_exception and handle_data_abort_exception, we
execute the fix_sp_lr function. It switches to system mode, 
stores the SP and LR registers and then switches back to abort
mode. This is necessary to store the correct SP and LR 
registers of the state before the exception was triggered. This
step is only required as we stay in abort mode instaed of 
directly switching back to system mode like the original 
firmware.

### handle_pref_abort_exception:
This is our handler for prefetch abort exceptions and is 
triggered by hardware breakpoints. It checks which breakpoint
triggered the exception, resets this breakpoint to trigger on
an instruction mismatch and increases a breakpoint hit counter.
If the instruction mismatch triggered the breakpoint, we reset
the it to trigger on the instrunction itself. If the hit counter
reaches a limit, we disable the breakpoint. Over all, we can 
have four hardware breakpoints and use the last one to handle 
instruction mismatch hits set by watchpoints.

### handle_data_abort_exception:
This is our handler for data abort exceptions and is triggered
by watchpoints. It cannot differentiate which of the four 
watchpoints was triggered, hence, we disable all and set an
instruction mismatch breakpoint on the address that triggered
the watchpoint. The breakpoint handler is then used to reset
the watchpoint.

## How to run the example?

To run the example, first load the patched firmware and driver:
```
make reloadfirmware FWPATCH=debugger_example
```

Then start printing the kernel log and grep for our output:
```
adb shell "su -c 'cat /proc/kmsg | grep NEX_TEST_IOCTL_1'"
```

Then, set up the wifi interface and print the firmware console:
```
adb shell "su -c 'ifconfig wlan0 down && ifconfig wlan0 up && \
  dhdutil -i wlan0 consoledump'"
```

The output of the firmware console should be:
```
start=0x001eb5cc len=0x00000800

RTE (USB-SDIO-CDC) 6.37.32.RC23.34.40 (r581243) on BCM4339 r1 @ 37.4/161.3/161.3MHz
000000.010 sdpcmdcdc0: Broadcom SDPCMD CDC driver
000000.018 reclaim section 0: Returned 48072 bytes to the heap
000000.066 debugger_example (21.04.2016 15:03:01)
000000.070 wl_nd_ra_filter_init: Enter..
000000.074 BP0(1) PC=001ecab0 LR=00180043 0=001df8c8 1=00000008 1=00000008 3=00000000 4=001df8c8 5=001d9acc 6=00180000
000000.084 BP0(1) PC=001ecab4 LR=00180043
000000.088 TCAM: 256 used: 198 exceed:0
000000.092 reclaim section 1: Returned 71844 bytes to the heap
000000.097 sdpcmd_dpc: Enable
000000.124 wl0: wlc_bmac_ucodembss_hwcap: Insuff mem for MBSS: templ memblks 192 fifo memblks 259
000000.239 wl0: wlc_enable_probe_req: state down, deferring setting of host flags
000000.358 wl0: wlc_enable_probe_req: state down, deferring setting of host flags
000000.366 wl0: wlc_enable_probe_req: state down, deferring setting of host flags
```