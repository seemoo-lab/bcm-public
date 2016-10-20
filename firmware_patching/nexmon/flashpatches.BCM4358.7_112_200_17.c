#include <firmware_version.h>
#include <patcher.h>

__attribute__((at(0x00005d90, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_0, 0x0018b322);

__attribute__((at(0x00001f78, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_1, 0x001eb808);

__attribute__((at(0x00005294, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_2, 0x0018b200);

__attribute__((at(0x000068ac, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_3, 0x0018b3ac);

__attribute__((at(0x000070c4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_4, 0x0018b6fc);

__attribute__((at(0x00007770, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_5, 0x0018b5cc);

__attribute__((at(0x0000803c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_6, 0x0018229c);

__attribute__((at(0x00008cbc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_7, 0x00182a44);

__attribute__((at(0x0000cf94, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_8, 0x0018cdb4);

__attribute__((at(0x0000d1c4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_9, 0x0018ccd2);

__attribute__((at(0x0000d540, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_10, 0x0018dfac);

__attribute__((at(0x0000d994, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_11, 0x0018d428);

__attribute__((at(0x0000e57c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_12, 0x0018f560);

__attribute__((at(0x0000e614, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_13, 0x0018ef48);

__attribute__((at(0x0000e640, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_14, 0x0018eec8);

__attribute__((at(0x0000e69c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_15, 0x0018fa08);

__attribute__((at(0x0000e778, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_16, 0x0018f620);

__attribute__((at(0x0000e860, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_17, 0x0019017e);

__attribute__((at(0x0000e870, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_18, 0x0018ea90);

__attribute__((at(0x0000e898, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_19, 0x0018edee);

__attribute__((at(0x0000e8a0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_20, 0x0018fc8c);

__attribute__((at(0x0000e9a4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_21, 0x00190002);

__attribute__((at(0x0000ea1c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_22, 0x0018ed6a);

__attribute__((at(0x0000ea2c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_23, 0x0019012a);

__attribute__((at(0x0000ea60, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_24, 0x0018fdb8);

__attribute__((at(0x0000eb14, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_25, 0x001901a6);

__attribute__((at(0x0000eb94, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_26, 0x0018ff50);

__attribute__((at(0x0000ebcc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_27, 0x0018ed7c);

__attribute__((at(0x0000ebe4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_28, 0x0018eb8e);

__attribute__((at(0x0000ecc8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_29, 0x0018eb58);

__attribute__((at(0x0000ed44, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_30, 0x0018f0aa);

__attribute__((at(0x0000ee6c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_31, 0x0018ef7c);

__attribute__((at(0x0000ef10, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_32, 0x0018f380);

__attribute__((at(0x0000f020, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_33, 0x0018f908);

__attribute__((at(0x0000f264, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_34, 0x0018f6e8);

__attribute__((at(0x0000f320, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_35, 0x0018f680);

__attribute__((at(0x0000f380, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_36, 0x0018fb74);

__attribute__((at(0x0000f65c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_37, 0x00190640);

__attribute__((at(0x0000f8c8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_38, 0x00190ea6);

__attribute__((at(0x0000fab8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_39, 0x00191cac);

__attribute__((at(0x0000ff98, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_40, 0x00191904);

__attribute__((at(0x000102a8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_41, 0x001910c0);

__attribute__((at(0x00010650, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_42, 0x00190760);

__attribute__((at(0x000116ac, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_43, 0x00193104);

__attribute__((at(0x00011784, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_44, 0x00193094);

__attribute__((at(0x000117b8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_45, 0x001930cc);

__attribute__((at(0x000117f0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_46, 0x001932f8);

__attribute__((at(0x00011898, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_47, 0x00193acc);

__attribute__((at(0x000118b8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_48, 0x00192e9c);

__attribute__((at(0x000118dc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_49, 0x00192e78);

__attribute__((at(0x00011900, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_50, 0x00193460);

__attribute__((at(0x000119d8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_51, 0x001933b0);

__attribute__((at(0x00011a60, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_52, 0x00193034);

__attribute__((at(0x00011aa8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_53, 0x00193224);

__attribute__((at(0x00062c14, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_54, 0x001bb6a4);

__attribute__((at(0x00004aa0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_55, 0x0018b058);

__attribute__((at(0x000147d4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_56, 0x0019487e);

__attribute__((at(0x00014ed8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_57, 0x00194840);

__attribute__((at(0x000171e0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_58, 0x00194e0c);

__attribute__((at(0x00017438, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_59, 0x00194d84);

__attribute__((at(0x000185dc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_60, 0x00199904);

__attribute__((at(0x000185f8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_61, 0x001999d8);

__attribute__((at(0x000179c4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_62, 0x00195c74);

__attribute__((at(0x000182bc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_63, 0x00195768);

__attribute__((at(0x000186b8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_64, 0x00199618);

__attribute__((at(0x000193fc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_65, 0x0019ec18);

__attribute__((at(0x0001cd78, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_66, 0x0019ff4c);

__attribute__((at(0x0001ce50, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_67, 0x0019f180);

__attribute__((at(0x0001da10, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_68, 0x0019a310);

__attribute__((at(0x0001dd90, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_69, 0x0019a1a8);

__attribute__((at(0x0001f1cc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_70, 0x0019edf6);

__attribute__((at(0x000218e8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_71, 0x0019ffea);

__attribute__((at(0x00021fd8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_72, 0x0019a7a4);

__attribute__((at(0x00031c74, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_73, 0x001a64de);

__attribute__((at(0x00023274, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_74, 0x001a1430);

__attribute__((at(0x00033eb8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_75, 0x001a6b38);

__attribute__((at(0x00034be0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_76, 0x001a6658);

__attribute__((at(0x00055adc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_77, 0x001b7afc);

__attribute__((at(0x000381dc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_78, 0x001ac5dc);

__attribute__((at(0x000232e4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_79, 0x0019ac60);

__attribute__((at(0x00038aec, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_80, 0x001ad858);

__attribute__((at(0x00038ea0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_81, 0x001ad030);

__attribute__((at(0x00038f08, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_82, 0x001ad330);

__attribute__((at(0x000390e4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_83, 0x001ad278);

__attribute__((at(0x00039244, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_84, 0x001ad07c);

__attribute__((at(0x00039a64, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_85, 0x001ad554);

__attribute__((at(0x00039cdc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_86, 0x001ad3ac);

__attribute__((at(0x00039ea0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_87, 0x001ad8e2);

__attribute__((at(0x0003aba8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_88, 0x001b08e0);

__attribute__((at(0x0003ae10, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_89, 0x001b0f4e);

__attribute__((at(0x0003b034, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_90, 0x001ae80c);

__attribute__((at(0x0003b338, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_91, 0x001aebd0);

__attribute__((at(0x0003c23c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_92, 0x001aff90);

__attribute__((at(0x0003d794, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_93, 0x001adbac);

__attribute__((at(0x0003dd08, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_94, 0x001af96c);

__attribute__((at(0x0003e548, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_95, 0x001af690);

__attribute__((at(0x00042dfc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_96, 0x001b1b24);

__attribute__((at(0x00044440, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_97, 0x001b141c);

__attribute__((at(0x00044bec, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_98, 0x001b15b8);

__attribute__((at(0x00045928, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_99, 0x001b2838);

__attribute__((at(0x00045d88, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_100, 0x001b25aa);

__attribute__((at(0x00045e90, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_101, 0x001b244c);

__attribute__((at(0x00045fec, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_102, 0x001b2720);

__attribute__((at(0x000460d4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_103, 0x001b28d8);

__attribute__((at(0x00046ca4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_104, 0x001b26c0);

__attribute__((at(0x00046de8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_105, 0x001b280e);

__attribute__((at(0x00047c00, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_106, 0x001b2c60);

__attribute__((at(0x00047c50, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_107, 0x001b2ea4);

__attribute__((at(0x00047cfc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_108, 0x001b2e88);

__attribute__((at(0x00047d74, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_109, 0x001b2ba8);

__attribute__((at(0x0003ec48, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_110, 0x001ae11c);

__attribute__((at(0x00049dc0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_111, 0x001b3370);

__attribute__((at(0x0004ba14, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_112, 0x001b35a4);

__attribute__((at(0x0004ed70, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_113, 0x001b36c0);

__attribute__((at(0x0004f7cc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_114, 0x001b37e0);

__attribute__((at(0x0004fc68, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_115, 0x001b4c28);

__attribute__((at(0x000500d8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_116, 0x001b478c);

__attribute__((at(0x00050160, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_117, 0x001b3ab2);

__attribute__((at(0x000501f4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_118, 0x001b513a);

__attribute__((at(0x000518e8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_119, 0x001b6c10);

__attribute__((at(0x0005024c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_120, 0x001b3828);

__attribute__((at(0x00051f14, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_121, 0x001b6dac);

__attribute__((at(0x00024c1c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_122, 0x001a3b9c);

__attribute__((at(0x00024c58, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_123, 0x0019fd88);

__attribute__((at(0x00029270, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_124, 0x001a3f48);

__attribute__((at(0x00050310, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_125, 0x001b51d4);

__attribute__((at(0x0004534c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_126, 0x001b13e8);

__attribute__((at(0x000491a0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_127, 0x001b2f5c);

__attribute__((at(0x0005319c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_128, 0x001b7390);

__attribute__((at(0x00053644, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_129, 0x001b7250);

__attribute__((at(0x00055380, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_130, 0x001b7778);

__attribute__((at(0x000553b4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_131, 0x001b77e4);

__attribute__((at(0x00050380, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_132, 0x001b3d78);

__attribute__((at(0x0005045c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_133, 0x001b3e38);

__attribute__((at(0x00050778, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_134, 0x001b3cc2);

__attribute__((at(0x0002ab94, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_135, 0x0019ee4e);

__attribute__((at(0x0002aca4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_136, 0x0019db98);

__attribute__((at(0x00055f50, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_137, 0x001b7e5e);

__attribute__((at(0x0005651c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_138, 0x001b82bc);

__attribute__((at(0x00056150, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_139, 0x001b806a);

__attribute__((at(0x00056304, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_140, 0x001b7fd6);

__attribute__((at(0x00056384, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_141, 0x001b8114);

__attribute__((at(0x0002b0f0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_142, 0x0019d2b0);

__attribute__((at(0x00056a4c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_143, 0x001b92be);

__attribute__((at(0x0002b6f4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_144, 0x001a018e);

__attribute__((at(0x00057f50, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_145, 0x001b937e);

__attribute__((at(0x0002ba28, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_146, 0x001a3adc);

__attribute__((at(0x000596a0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_147, 0x001b97f4);

__attribute__((at(0x0003f9a4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_148, 0x001afcde);

__attribute__((at(0x0005992c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_149, 0x001ba068);

__attribute__((at(0x00059b3c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_150, 0x001ba812);

__attribute__((at(0x00059c28, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_151, 0x001b9c62);

__attribute__((at(0x0005b26c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_152, 0x001b9e10);

__attribute__((at(0x0005c660, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_153, 0x001ba280);

__attribute__((at(0x0005d264, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_154, 0x001baafc);

__attribute__((at(0x0005d734, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_155, 0x001ba8e8);

__attribute__((at(0x0005d774, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_156, 0x001ba920);

__attribute__((at(0x0005da14, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_157, 0x001baab0);

__attribute__((at(0x0005db00, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_158, 0x001bac34);

__attribute__((at(0x0005e6cc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_159, 0x001bacb0);

__attribute__((at(0x00061630, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_160, 0x001baf64);

__attribute__((at(0x00062bb4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_161, 0x001bb1ca);

__attribute__((at(0x000634f0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_162, 0x001d5184);

__attribute__((at(0x00064a94, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_163, 0x001d5442);

__attribute__((at(0x0002ccf0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_164, 0x0019ac14);

__attribute__((at(0x0002cd30, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_165, 0x0019ebf4);

__attribute__((at(0x0002ce10, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_166, 0x0019d28c);

__attribute__((at(0x00063370, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_167, 0x001bb372);

__attribute__((at(0x0006632c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_168, 0x001d57cc);

__attribute__((at(0x000665d8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_169, 0x001d567c);

__attribute__((at(0x00066774, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_170, 0x001d56d0);

__attribute__((at(0x000667dc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_171, 0x001d5a24);

__attribute__((at(0x0002d14c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_172, 0x001a12e0);

__attribute__((at(0x00066d18, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_173, 0x001d5cf4);

__attribute__((at(0x00066e40, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_174, 0x001d5c96);

__attribute__((at(0x00066eec, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_175, 0x001d5c3e);

__attribute__((at(0x00050dc8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_176, 0x001b382c);

__attribute__((at(0x00050e7c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_177, 0x001b4f54);

__attribute__((at(0x0002d314, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_178, 0x0019f0bc);

__attribute__((at(0x0002d3c8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_179, 0x0019f044);

__attribute__((at(0x00067df0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_180, 0x001d6174);

__attribute__((at(0x00068214, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_181, 0x001d690c);

__attribute__((at(0x00068994, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_182, 0x001d61ac);

__attribute__((at(0x0006996c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_183, 0x001d729c);

__attribute__((at(0x00069a38, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_184, 0x001d6f54);

__attribute__((at(0x00067598, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_185, 0x001d5e10);

__attribute__((at(0x00067650, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_186, 0x001d5d50);

__attribute__((at(0x00050f10, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_187, 0x001b3c62);

__attribute__((at(0x00052b10, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_188, 0x001b6fdc);

__attribute__((at(0x00052c3c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_189, 0x001b70f0);

__attribute__((at(0x000388bc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_190, 0x001ac70c);

__attribute__((at(0x00051030, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_191, 0x001b501a);

__attribute__((at(0x0003fbd0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_192, 0x001afe18);

__attribute__((at(0x0003fdcc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_193, 0x001ae24c);

__attribute__((at(0x0003fed8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_194, 0x001ae61c);

__attribute__((at(0x00040000, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_195, 0x001ae74a);

__attribute__((at(0x00040098, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_196, 0x001afd68);

__attribute__((at(0x0004013c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_197, 0x001ae592);

__attribute__((at(0x0004022c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_198, 0x001b1184);

__attribute__((at(0x00070068, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_199, 0x001d7b8e);

__attribute__((at(0x0006fa6c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_200, 0x001d7358);

__attribute__((at(0x00070538, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_201, 0x001d7b3a);

__attribute__((at(0x00070a74, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_202, 0x001d7a30);

__attribute__((at(0x000714cc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_203, 0x001d826c);

__attribute__((at(0x00071844, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_204, 0x001d8204);

__attribute__((at(0x00072758, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_205, 0x001d86a6);

__attribute__((at(0x00071d68, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_206, 0x001d81a4);

__attribute__((at(0x000721b8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_207, 0x001d7ffc);

__attribute__((at(0x0002f784, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_208, 0x001a0e74);

__attribute__((at(0x00051158, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_209, 0x001b3962);

__attribute__((at(0x0002f9fc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_210, 0x0019d968);

__attribute__((at(0x00055b64, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_211, 0x001b7a80);

__attribute__((at(0x00074748, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_212, 0x001d8ace);

__attribute__((at(0x00074fe4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_213, 0x001d8b30);

__attribute__((at(0x000763ec, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_214, 0x001d8cda);

__attribute__((at(0x000766e0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_215, 0x001d8b8c);

__attribute__((at(0x000767bc, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_216, 0x001d8e26);

__attribute__((at(0x00076948, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_217, 0x001d8c68);

__attribute__((at(0x0007a194, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_218, 0x001dadac);

__attribute__((at(0x0007a674, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_219, 0x001d976a);

__attribute__((at(0x0007b2f4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_220, 0x001da82a);

__attribute__((at(0x0007bcec, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_221, 0x001d9070);

__attribute__((at(0x0007bd48, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_222, 0x001d957a);

__attribute__((at(0x0007bd88, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_223, 0x001d904a);

__attribute__((at(0x0007c528, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_224, 0x001d9a74);

__attribute__((at(0x0007c954, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_225, 0x001da350);

__attribute__((at(0x0007cbf0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_226, 0x001d9d46);

__attribute__((at(0x0007cec0, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_227, 0x001da032);

__attribute__((at(0x0007d160, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_228, 0x001da608);

__attribute__((at(0x0007d28c, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_229, 0x001d9908);

__attribute__((at(0x0007f700, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_230, 0x001da740);

__attribute__((at(0x0007f954, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_231, 0x001dafd4);

__attribute__((at(0x000806e4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_232, 0x001db154);

__attribute__((at(0x00030918, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_233, 0x0019ef74);

__attribute__((at(0x00081344, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_234, 0x001db40a);

__attribute__((at(0x000512a4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_235, 0x001b3c3a);

__attribute__((at(0x00051304, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_236, 0x001b3c18);

__attribute__((at(0x00031568, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_237, 0x0019ce3c);

__attribute__((at(0x000840a4, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_238, 0x001db904);

__attribute__((at(0x00031604, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_239, 0x0019ab6a);

__attribute__((at(0x00085890, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_240, 0x001dbb64);

__attribute__((at(0x00085ea8, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_241, 0x001dbcf2);

__attribute__((at(0x000860ec, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_242, 0x001dbc1c);

__attribute__((at(0x00087944, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_243, 0x001dbfcc);

__attribute__((at(0x00031844, "flashpatch", CHIP_VER_BCM4358, FW_VER_7_112_200_17)))
BPatch(flash_patch_244, 0x001a0f86);

