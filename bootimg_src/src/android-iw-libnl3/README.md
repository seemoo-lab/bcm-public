Porting iw on android.

Using libnl-3 code: https://github.com/dschuermann/libnl-3-android

Using iw code: http://git.kernel.org/cgit/linux/kernel/git/jberg/iw.git

cd android\_toolchain

ndk-build

you will get iw , libnl-3.so and libnl-genl-3.so at libs/\<arch\> directory.
