![NexMon logo](https://dev.seemoo.tu-darmstadt.de/bcm/bcm-public/raw/master/logo/nexmon-logo-color.png)

## What is NexMon?

NexMon enables the monitor mode of the bcm4339 Wi-Fi chip on the Nexus 5.

## WARNING

Our software may damage your hardware and may void your hardware’s warranty! You use our tools at your own risk and responsibility! If you don't like these terms, don't use nexmon!

## What this Repo contains

* A bootable boot ROM for Android
* Everything needed to build the ROM by yourself

## Requirements

* Nexus 5 with bcm4339 chipset
* The phone needs to be rooted with [Chainfire's SuperSU v2.66 (system-less)](http://forum.xda-developers.com/showpost.php?p=64161125&postcount=3)
* Android 6.0 or 6.0.1
* Android Tools (adb + fastboot)

## Steps needed to run the boot.img on your phone

* Download the [boot.img](https://dev.seemoo.tu-darmstadt.de/bcm/bcm-public/raw/master/boot.img) (this project is still in heavy development, the monitor mode should work on the boot.img in this commit though: 65ce51f3363135176a870338ab24137d1151d692)
* `adb reboot bootloader`
* `fastboot boot boot.img`
  * without the `flash` parameter, this boot image will be reset to the previous one on the next reboot
* `fastboot reboot`
* `adb shell`
* `su -`
* `insmod /nexmon/nexmon.ko`
* `ifconfig wlan0 up`
* do whatever you want, e.g. run tcpdump: `/nexmon/bin/tcpdump -i wlan0 -s0`

## Steps to build your own boot.img
* `source setup_env.sh`
* `make cleanboot`
* `make boot.img`

## Caveats
* The current patch is unable to handle aggregated frames (A-MSDU's). This makes it impossible to receive frames in an 802.11n/802.11ac enabled environment. 

## Read our paper

Feel free to read and reference our paper on the development of this project [NexMon: A Cookbook for Firmware Modifications on Smartphones to Enable Monitor Mode](http://arxiv.org/abs/1601.07077)

### Abstract

Full control over a Wi-Fi chip for research purposes is often limited by its
firmware, which makes it hard to evolve communication protocols and test schemes
in practical environments. Monitor mode, which allows eavesdropping on all
frames on a wireless communication channel, is a first step to lower this
barrier. Use cases include, but are not limited to, network packet analyses,
security research and testing of new medium access control layer protocols.
Monitor mode is generally offered by SoftMAC drivers that implement the media
access control sublayer management entity (MLME) in the driver rather than in
the Wi-Fi chip. On smartphones, however, mostly FullMAC chips are used to reduce
power consumption, as MLME tasks do not need to wake up the main processor. Even
though, monitor mode is also possible in FullMAC scenarios, it is generally not
implemented in today's Wi-Fi firmwares used in smartphones.
This work focuses on bringing monitor mode to Nexus 5 smartphones to enhance the
interoperability between applications that require monitor mode and BCM4339
Wi-Fi chips. The implementation is based on our new C-based programming
framework to extend existing Wi-Fi firmwares.
