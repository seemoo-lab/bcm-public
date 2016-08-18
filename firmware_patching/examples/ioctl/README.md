# ioctl_example

This example demonstrates the exchange of ioctl messages between
firmware and driver. The driver can trigger the transmission of
an ioctl with a given command number and a buffer to exchange
information with the firmware. In the driver, the buffer can be
filled and the handler in the firmware can read the information
(SET operation). Alternatively, the ioctl caller can request 
information from the firmware (GET operation). The caller passes
an empty or prefilled buffer to the firmware and the firmware
can modify this buffer.

## How to run the example?

After sending the WLC_UP ioctl in the dhd_cfg80211.c file, we 
send two additional ioctls. The first one (NEX_TEST_IOCTL_1) 
gets a string from the firmware and prints it to the kernel log.
The second one (NEX_TEST_IOCTL_2) sends a string to the firmware
which prints it on its console.

To run the example, first load the patched firmware and driver:
```
make reloadfirmware FWPATCH=ioctl_example
```

Then start printing the kernel log and grep for our output:
```
adb shell "su -c 'cat /proc/kmsg | grep NEX_TEST_IOCTL_1'"
```

In a different terminal window you can set up the wifi interface
and print the firmware console:
```
adb shell "su -c 'ifconfig wlan0 down && ifconfig wlan0 up && \
  dhdutil -i wlan0 consoledump'"
```

The output of the kernel log should be:
```
<4>[20339.600177] NEX_TEST_IOCTL_1: hello driver!
```

The output of the firmware console should be:
```
start=0x001eb5cc len=0x00000800

RTE (USB-SDIO-CDC) 6.37.32.RC23.34.40 (r581243) on BCM4339 r1 @ 37.4/161.3/161.3MHz
000000.010 sdpcmdcdc0: Broadcom SDPCMD CDC driver
000000.017 reclaim section 0: Returned 48072 bytes to the heap
000000.066 ioctl_example (21.04.2016 12:02:19)
000000.070 wl_nd_ra_filter_init: Enter..
000000.074 TCAM: 256 used: 198 exceed:0
000000.078 reclaim section 1: Returned 71844 bytes to the heap
000000.083 sdpcmd_dpc: Enable
000000.092 wl0: wlc_bmac_ucodembss_hwcap: Insuff mem for MBSS: templ memblks 192 fifo memblks 259
000000.133 wl0: wlc_enable_probe_req: state down, deferring setting of host flags
000000.199 wl0: wlc_enable_probe_req: state down, deferring setting of host flags
000000.207 wl0: wlc_enable_probe_req: state down, deferring setting of host flags
000000.216 nex_ioctl_handler_in_c: cmd=15, arg=0022ded8, len=64
000000.222 nex_ioctl_handler_in_c: cmd=16, arg=0022d750, len=64
000000.227 NEX_TEST_IOCTL_2: hello firmware!
```
