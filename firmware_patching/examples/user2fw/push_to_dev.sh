#!/bin/bash

adb push fw_bcmdhd.bin /sdcard/fw_bcmdhd_patched.bin
#adb shell "su -c mount -o rw,remount /system"
#adb shell "su -c cp /sdcard/fw_bcmdhd_patched.bin /vendor/firmware/fw_bcmdhd.bin"
