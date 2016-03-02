# User space tool to send data to firmware via kernel

* This tool uses the netlink kernel interface to transmit data from the user to the kernel space
* pass a parameter to send a single string to the firmware
* TODO handle length of data appropriately (its just strlen() for now)