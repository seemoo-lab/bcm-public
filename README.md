![NexMon logo](https://dev.seemoo.tu-darmstadt.de/bcm/bcm-public/raw/master/logo/nexmon-logo-color.png)

## What is NexMon?

NexMon enables the monitor mode of the bcm4339 Wi-Fi chip on the Nexus 5.

## What this Repo contains

* A bootable boot ROM for Android
* Everything needed to build the ROM by yourself

## Requirements

* Nexus 5 with bcm4339 chipset
* The phone needs to be rooted
* Android 6.0 or 6.0.1
* Android Tools (adb + fastboot)

## Steps needed to run the boot.img on your phone

* Download the [boot.img](https://dev.seemoo.tu-darmstadt.de/bcm/bcm-public/raw/master/boot.img)
* `adb reboot bootloader`
* `fastboot boot boot.img`
  * without the `flash` parameter, this boot image will be reset to the previous one on the next reboot
* `fastboot reboot`
* `adb shell`
* `su -`
* `insmod /nexmon/bcmdhd.ko`
* `ifconfig wlan0 up`
* do whatever you want, e.g. run tcpdump: `/nexmon/bin/tcpdump -i wlan0 -s0`