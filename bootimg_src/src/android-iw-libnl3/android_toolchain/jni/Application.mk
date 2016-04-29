# Optimizations
#APP_OPTIM := release
APP_OPTIM := debug

# Build target
#APP_ABI := armeabi
APP_ABI := all
#APP_ABI := armeabi armeabi-v7a x86 mips

# Target platform level, this defines the included sysroot from Android NDK
APP_PLATFORM := android-5

#APP_MODULES := nl-3 nl-genl-3 nl-route-3
APP_MODULES := nl-3 nl-genl-3 iw
#APP_MODULES := nl-3 nl-genl-3 nl-nf-3 nl-route-3
