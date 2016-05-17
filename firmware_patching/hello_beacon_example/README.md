# hello_beacon_example

This example creates beacon frames and sends them out over 
WiFi announcing a NEXMON WiFi on channel 1.

## How to run the example?

To run the example, first load the patched firmware and driver:
```
make reloadfirmware FWPATCH=hello_beacon_example
```

You need to run Wireshark or tcpdump on another device with a
WiFi interface on channel 1 in monitor mode and filter for the
MAC address cc:cc:cc:cc:cc:cc, for example, on a Mac OS X 
device:
```
sudo airport -z -c1
tcpdump -In -i en0 -s0 ether host cc:cc:cc:cc:cc:cc
```

Then set up the WiFi interface on the Nexus 5:
```
adb shell "su -c 'ifconfig wlan0 up'"
```

The output of tcpdump should be:
```
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on en0, link-type IEEE802_11_RADIO (802.11 plus radiotap header), capture size 262144 bytes
02:20:00.754065 74847966us tsft 1.0 Mb/s 2412 MHz 11g -47dB signal -84dB noise antenna 0 Beacon (NEXMON) ESS
```
