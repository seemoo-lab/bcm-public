#!/bin/bash
ndk-build

adb push libs/armeabi-v7a/user2fw sdcard/
adb shell "su -c 'cp /sdcard/user2fw /data/'"
adb shell "su -c 'chmod 777 /data/user2fw'"
#adb shell "su -c /data/user2fw"
