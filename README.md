![NexMon logo](https://dev.seemoo.tu-darmstadt.de/bcm/bcm-public/raw/master/logo/nexmon-logo-color.png)

## What is NexMon?

NexMon is a firmware patching framework for the BCM4339 WiFi firmware of Nexus 5 smartphones. It's main intension was to enable monitor mode and frame injection, which is already working quite well. Nevertheless, we also publish many example patches that allow to dive into firmware reverse engineering and firmware patching. Especially, the ability to write firmware patches in C makes it easy to use, also for beginenrs.

If you only intend to activate monitor mode on a device using the bcmdhd driver, take a look at the following commit: https://dev.seemoo.tu-darmstadt.de/bcm/bcm-public/commit/ed11c18aa89cd0374a3ce7655337cc0e4d1cdd0a

## WARNING

Our software may damage your hardware and may void your hardware’s warranty! You use our tools at your own risk and responsibility! If you don't like these terms, don't use nexmon!

## What this Repo contains

* A bootable boot ROM for Android
* Everything needed to build the ROM by yourself

## Requirements

* Nexus 5 with bcm4339 chipset
* Your bootloader needs to be unlocked (no root required)
* Android 6.0 or 6.0.1
* Android Tools (adb + fastboot)

### To build your own patches

* Xubuntu 14.04.4 64-Bit (do not use a different version to avoid compilation problems)
* set the environment variable `NDK_ROOT` to an installation of android-ndk-r11c (using exactly this version allows to build all tools without problems)

## Steps needed to run the boot.img on your phone

* Download the [boot.img](https://dev.seemoo.tu-darmstadt.de/bcm/bcm-public/raw/master/boot.img) (this project is still in heavy development, the monitor mode should work on the boot.img in this commit though: ~~65ce51f3363135176a870338ab24137d1151d692~~ ~~1171d135c0187d07156dc83ee76aeb3b98894d4c~~ ~~[bb562a75](https://dev.seemoo.tu-darmstadt.de/bcm/bcm-public/raw/bb562a75acbf0d9cd1d90f2f5e5818de98e0e42a/boot.img)~~ [2c39825](https://dev.seemoo.tu-darmstadt.de/bcm/bcm-public/raw/2c3982518c89eb9580f868b2337c944d8405bf4a/boot.img))
* `adb reboot bootloader`
* `fastboot boot boot.img`
  * without the `flash` parameter, this boot image will be reset to the previous one on the next reboot
* `fastboot reboot`
* `adb shell`
* `su`
* `insmod /nexmon/bcmdhd.ko`
* `ifconfig wlan0 up`
* `nexutil -m true` this tells the firmware to activate monitor mode
* do whatever you want, e.g. run tcpdump: `/nexmon/bin/tcpdump -i wlan0 -s0`

## Steps to build your own boot.img
* `source setup_env.sh`
* `make cleanboot`
* `make boot.img`

## Steps to create your own firmware patches
* see the [firmware patching folder](https://dev.seemoo.tu-darmstadt.de/bcm/bcm-public/tree/master/firmware_patching)

## Caveats
* ~~The current patch is unable to handle aggregated frames (A-MSDU's). This makes it impossible to receive frames in an 802.11n/802.11ac enabled environment.~~
 * Current Monitor Mode was tested and works for 802.11n with 20MHz wide channels (2.4 as well as 5GHz), ~~40MHz channels seems to be a problem. We keep working on it ;-)~~
 * Using this [patch](https://dev.seemoo.tu-darmstadt.de/bcm/bcm-public/blob/90bed6e1c3ad70ddc23ccf44033b152e0db300b6/kernel_patches/40mhz_channels_5GHz.patch) should enalbe `iw` to set 40MHz channels. It currently only works for 5GHz frequencies. 
   Capturing WiFi packets which use mutliple spatial streams is still not possible, this is caused by a hardware limitation of the Nexus 5

## Read our papers

Feel free to read and reference our papers on the development of this project 

* M. Schulz, D. Wegemer and M. Hollick. [NexMon: A Cookbook for Firmware 
Modifications on Smartphones to Enable Monitor Mode]
(http://arxiv.org/abs/1601.07077), CoRR, vol. abs/1601.07077, December 2015. 
[bibtex](http://dblp.uni-trier.de/rec/bibtex/journals/corr/SchulzWH16)
* M. Schulz, D. Wegemer, M. Hollick. [DEMO: Using NexMon, the C-based WiFi 
firmware modification framework](https://dl.acm.org/citation.cfm?id=2942419), 
Proceedings of the 9th ACM Conference on Security and Privacy in Wireless and 
Mobile Networks, WiSec 2016, July 2016.

## Contact

* [Matthias Schulz](https://seemoo.tu-darmstadt.de/mschulz) <mschulz@seemoo.tu-darmstadt.de>
* Daniel Wegemer <dwegemer@seemoo.tu-darmstadt.de>
