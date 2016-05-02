# filterutil: upload pcap-filters directly to the firmware
* needs 'nexmon' driver
* based on libpcap (also in this repo)

# Usage:
* filter which consists of multiple commands need to be surrounded by quotes
* e.g.: `filterutil "wlan addr1 ff:ff:ff:ff:ff:ff"`
* see here which filter combinations are possible: http://www.tcpdump.org/manpages/pcap-filter.7.html
