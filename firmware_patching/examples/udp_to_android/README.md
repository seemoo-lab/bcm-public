# udp_to_android_example

This example firmware patch packs a payload into a UDP frame and
sends it over the SDIO interface to Android. The UDP frame is 
encapsulated into an IPv6 frame and an Ethernet frame which are
both addressed to broadcast addresses. The UDP frame is actually
a UDPlite frame that only has a checksum over its header and not
the payload. Tcpdump can be used to show the frame in Android.
The frame transmission is triggered by a call to the hooked 
wlc_ioctl function that is called to dispatch ioctls (see also
the ioctl_example).

## How to run the example?

To run the example, first load the patched firmware and driver:
```
make reloadfirmware FWPATCH=udp_to_android_example
```

Then set up the wifi interface and print the firmware console:
```
adb shell "su -c 'ifconfig wlan0 down && ifconfig wlan0 up && \
  dhdutil -i wlan0 consoledump'"
```

Then start tcpdump in a new terminal:
```
adb shell "su -c 'tcpdump -i wlan0 -s0'"
```

Tcpdump's output should be as follows:
```
tcpdump: WARNING: wlan0: no IPv4 address assigned
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on wlan0, link-type EN10MB (Ethernet), capture size 65535 bytes
08:13:53.553943 4e:45:58:4d:4f:4e (oui Unknown) > Broadcast, ethertype IPv6 (0x86dd), length 77: 
  0x0000:  6000 0000 0017 8801 ff02 0000 0000 0000  `...............
  0x0010:  0000 0000 0000 0001 ff02 0000 0000 0000  ................
  0x0020:  0000 0000 0000 0001 d6d8 d6d8 0008 5246  ..............RF
  0x0030:  4865 6c6c 6f20 416e 6472 6f69 6421 00    Hello.Android!.
08:13:53.558046 4e:45:58:4d:4f:4e (oui Unknown) > Broadcast, ethertype IPv6 (0x86dd), length 77: 
  0x0000:  6000 0000 0017 8801 ff02 0000 0000 0000  `...............
  0x0010:  0000 0000 0000 0001 ff02 0000 0000 0000  ................
  0x0020:  0000 0000 0000 0001 d6d8 d6d8 0008 5246  ..............RF
  0x0030:  4865 6c6c 6f20 416e 6472 6f69 6421 00    Hello.Android!.
```

If you do not see any output, generate some ioctls. For example,
by triggering a scan for WiFi access points:
```
adb shell "su -c 'iwlist wlan0 scan'"
```