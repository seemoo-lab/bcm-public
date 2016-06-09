# bootimg_src directory

This directory contains files that go into the boot.img. According 
to our open source philosophy, we intend to build all tools deliered
with NexMon from source. The sources lie in the `src` directory.
Calling `make` in the `src` directory build all tools delivered with
NexMon and copies them into the `bin` directory. From where they 
will be taken by the main make file to be included in the boot.img.
The firmware directory contains firmwares for the BCM4339 WiFi chip
that come with the stock Android firmware. The root directory 
contains binaries that go into the root directory of the ramdisk 
included in our boot.img. Currently, building the init binary is not
included in the build process, but will be integrated in the future.
Hence, we deliver it as binary. Additionally, boot.img is a dump of 
the boot partition of a Hammerhead MOB30M factory image. It will be
extracted and used as a base for our boot.img. In the future, this
could also be built from source.