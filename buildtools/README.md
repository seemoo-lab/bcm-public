# buildtools directory

This directory contains all tools required to build the boot.img
and patch the WiFi firmware. Besides the arm-eabi-47 arm compiler,
all tools are built from source. 

* b43 is a compiler and decompiler for the firmware of the D11 core.
* binary_patcher is a python script to patch firmware binaries.
* elffile is a python script to read elf files and extract information about sections.
* mapaddr is a tool to analyze stack dumps of the BCM4339 WiFi firmware
* mkboot contains tools to extract and compile boot.img files