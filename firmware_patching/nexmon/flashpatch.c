#include <firmware_version.h>
#include <patcher.h>

__attribute__((at(0x0000fd1c, "flashpatch")))
BPatch(flash_patch_0, 0x001838c2);

__attribute__((at(0x00013a78, "flashpatch")))
BPatch(flash_patch_1, 0x00183fe4);

__attribute__((at(0x0005a9e0, "flashpatch")))
BPatch(flash_patch_2, 0x001ad760);

__attribute__((at(0x0005a9e8, "flashpatch")))
BPatch(flash_patch_3, 0x001ad764);

__attribute__((at(0x0005d4e0, "flashpatch")))
BPatch(flash_patch_4, 0x001af0e2);

__attribute__((at(0x0005d62c, "flashpatch")))
BPatch(flash_patch_5, 0x001af174);

__attribute__((at(0x0005d944, "flashpatch")))
BPatch(flash_patch_6, 0x001af3fc);

__attribute__((at(0x0005dc50, "flashpatch")))
BPatch(flash_patch_7, 0x001af880);

__attribute__((at(0x000169b8, "flashpatch")))
BPatch(flash_patch_8, 0x00181dd4);

__attribute__((at(0x00016a8c, "flashpatch")))
BPatch(flash_patch_9, 0x00181df0);

__attribute__((at(0x0001673c, "flashpatch")))
BPatch(flash_patch_10, 0x001814f4);

__attribute__((at(0x00016aa8, "flashpatch")))
BPatch(flash_patch_11, 0x00181d14);

__attribute__((at(0x0001690c, "flashpatch")))
BPatch(flash_patch_12, 0x00181a14);

__attribute__((at(0x00010444, "flashpatch")))
BPatch(flash_patch_13, 0x001831e8);

__attribute__((at(0x0001aff0, "flashpatch")))
BPatch(flash_patch_14, 0x00182acc);

__attribute__((at(0x0001b3cc, "flashpatch")))
BPatch(flash_patch_15, 0x001828b4);

__attribute__((at(0x0001d34c, "flashpatch")))
BPatch(flash_patch_16, 0x00185250);

__attribute__((at(0x0001ddec, "flashpatch")))
BPatch(flash_patch_17, 0x00185248);

__attribute__((at(0x000248d8, "flashpatch")))
BPatch(flash_patch_18, 0x00185c24);

__attribute__((at(0x00027088, "flashpatch")))
BPatch(flash_patch_19, 0x00189752);

__attribute__((at(0x00025950, "flashpatch")))
BPatch(flash_patch_20, 0x001861c0);

__attribute__((at(0x00025994, "flashpatch")))
BPatch(flash_patch_21, 0x001861e4);

__attribute__((at(0x00025ef4, "flashpatch")))
BPatch(flash_patch_22, 0x00186008);

__attribute__((at(0x00027cd8, "flashpatch")))
BPatch(flash_patch_23, 0x00190d70);

__attribute__((at(0x00028dfc, "flashpatch")))
BPatch(flash_patch_24, 0x0019096c);

__attribute__((at(0x00028e28, "flashpatch")))
BPatch(flash_patch_25, 0x0018a224);

__attribute__((at(0x000299e8, "flashpatch")))
BPatch(flash_patch_26, 0x0018a10c);

__attribute__((at(0x00029c20, "flashpatch")))
BPatch(flash_patch_27, 0x00191528);

__attribute__((at(0x0002afac, "flashpatch")))
BPatch(flash_patch_28, 0x00195238);

__attribute__((at(0x0002be38, "flashpatch")))
BPatch(flash_patch_29, 0x0018d738);

__attribute__((at(0x0002c864, "flashpatch")))
BPatch(flash_patch_30, 0x00193670);

__attribute__((at(0x0002f9bc, "flashpatch")))
BPatch(flash_patch_31, 0x00194608);

__attribute__((at(0x0006d518, "flashpatch")))
BPatch(flash_patch_32, 0x001c83ea);

__attribute__((at(0x0003cbf0, "flashpatch")))
BPatch(flash_patch_33, 0x0019f318);

__attribute__((at(0x0003cc18, "flashpatch")))
BPatch(flash_patch_34, 0x0019f46a);

__attribute__((at(0x0003cc70, "flashpatch")))
BPatch(flash_patch_35, 0x0019f350);

__attribute__((at(0x00030a78, "flashpatch")))
BPatch(flash_patch_36, 0x00190ef2);

__attribute__((at(0x0003d358, "flashpatch")))
BPatch(flash_patch_37, 0x001a2968);

__attribute__((at(0x0003f848, "flashpatch")))
BPatch(flash_patch_38, 0x001a2dc8);

__attribute__((at(0x0003f954, "flashpatch")))
BPatch(flash_patch_39, 0x001a3e06);

__attribute__((at(0x0003fc04, "flashpatch")))
BPatch(flash_patch_40, 0x001a34a0);

__attribute__((at(0x000433f4, "flashpatch")))
BPatch(flash_patch_41, 0x001a5a64);

__attribute__((at(0x000441b8, "flashpatch")))
BPatch(flash_patch_42, 0x001a6134);

__attribute__((at(0x000443c4, "flashpatch")))
BPatch(flash_patch_43, 0x001a601c);

__attribute__((at(0x00044a44, "flashpatch")))
BPatch(flash_patch_44, 0x001a68da);

__attribute__((at(0x00044b14, "flashpatch")))
BPatch(flash_patch_45, 0x001a65c8);

__attribute__((at(0x00044d00, "flashpatch")))
BPatch(flash_patch_46, 0x001a669a);

__attribute__((at(0x00044e50, "flashpatch")))
BPatch(flash_patch_47, 0x001a6d3c);

__attribute__((at(0x00045748, "flashpatch")))
BPatch(flash_patch_48, 0x001a8338);

__attribute__((at(0x00045e4c, "flashpatch")))
BPatch(flash_patch_49, 0x001a79b4);

__attribute__((at(0x000463a0, "flashpatch")))
BPatch(flash_patch_50, 0x001a98f8);

__attribute__((at(0x000466b4, "flashpatch")))
BPatch(flash_patch_51, 0x001a70ea);

__attribute__((at(0x000469f8, "flashpatch")))
BPatch(flash_patch_52, 0x001a6ed8);

__attribute__((at(0x00047af4, "flashpatch")))
BPatch(flash_patch_53, 0x001a8c44);

__attribute__((at(0x000488ec, "flashpatch")))
BPatch(flash_patch_54, 0x001a8488);

__attribute__((at(0x00031418, "flashpatch")))
BPatch(flash_patch_55, 0x0018edd0);

__attribute__((at(0x0003218c, "flashpatch")))
BPatch(flash_patch_56, 0x0019065e);

__attribute__((at(0x000321d8, "flashpatch")))
BPatch(flash_patch_57, 0x001906b8);

__attribute__((at(0x0004d778, "flashpatch")))
BPatch(flash_patch_58, 0x001ab12c);

__attribute__((at(0x0004d914, "flashpatch")))
BPatch(flash_patch_59, 0x001ab04c);

__attribute__((at(0x0004dc7c, "flashpatch")))
BPatch(flash_patch_60, 0x001ab2d0);

__attribute__((at(0x0004e1a4, "flashpatch")))
BPatch(flash_patch_61, 0x001ab24c);

__attribute__((at(0x0004e1d8, "flashpatch")))
BPatch(flash_patch_62, 0x001ab27e);

__attribute__((at(0x0004eb04, "flashpatch")))
BPatch(flash_patch_63, 0x001ab720);

__attribute__((at(0x0004f288, "flashpatch")))
BPatch(flash_patch_64, 0x001aae1a);

__attribute__((at(0x0004f7a4, "flashpatch")))
BPatch(flash_patch_65, 0x001aaee8);

__attribute__((at(0x000323f0, "flashpatch")))
BPatch(flash_patch_66, 0x0018c248);

__attribute__((at(0x0003256c, "flashpatch")))
BPatch(flash_patch_67, 0x0019b2f4);

__attribute__((at(0x000326ac, "flashpatch")))
BPatch(flash_patch_68, 0x001908b2);

__attribute__((at(0x00050d10, "flashpatch")))
BPatch(flash_patch_69, 0x001ac50c);

__attribute__((at(0x00050e14, "flashpatch")))
BPatch(flash_patch_70, 0x001abb74);

__attribute__((at(0x00050f2c, "flashpatch")))
BPatch(flash_patch_71, 0x001abd8e);

__attribute__((at(0x00051070, "flashpatch")))
BPatch(flash_patch_72, 0x001abb9c);

__attribute__((at(0x0005120c, "flashpatch")))
BPatch(flash_patch_73, 0x001ac420);

__attribute__((at(0x00051310, "flashpatch")))
BPatch(flash_patch_74, 0x001ac58c);

__attribute__((at(0x00051828, "flashpatch")))
BPatch(flash_patch_75, 0x001ac1fc);

__attribute__((at(0x000523c0, "flashpatch")))
BPatch(flash_patch_76, 0x001abed0);

__attribute__((at(0x000494a0, "flashpatch")))
BPatch(flash_patch_77, 0x001a757e);

__attribute__((at(0x00056904, "flashpatch")))
BPatch(flash_patch_78, 0x001acaa8);

__attribute__((at(0x00057110, "flashpatch")))
BPatch(flash_patch_79, 0x001ac86c);

__attribute__((at(0x00057294, "flashpatch")))
BPatch(flash_patch_80, 0x001ac7e8);

__attribute__((at(0x00057ac8, "flashpatch")))
BPatch(flash_patch_81, 0x001ad1f2);

__attribute__((at(0x0005a3d0, "flashpatch")))
BPatch(flash_patch_82, 0x001ad348);

__attribute__((at(0x0005ba28, "flashpatch")))
BPatch(flash_patch_83, 0x001adbb4);

__attribute__((at(0x0005bbf8, "flashpatch")))
BPatch(flash_patch_84, 0x001ad76c);

__attribute__((at(0x0005bc88, "flashpatch")))
BPatch(flash_patch_85, 0x001ae40c);

__attribute__((at(0x0005bdd0, "flashpatch")))
BPatch(flash_patch_86, 0x001ada4c);

__attribute__((at(0x0005bf28, "flashpatch")))
BPatch(flash_patch_87, 0x001ad768);

__attribute__((at(0x0005ece4, "flashpatch")))
BPatch(flash_patch_88, 0x001b03f8);

__attribute__((at(0x0005ef90, "flashpatch")))
BPatch(flash_patch_89, 0x001b0154);

__attribute__((at(0x0005f1dc, "flashpatch")))
BPatch(flash_patch_90, 0x001b0040);

__attribute__((at(0x0005f520, "flashpatch")))
BPatch(flash_patch_91, 0x001afea0);

__attribute__((at(0x0005f5d0, "flashpatch")))
BPatch(flash_patch_92, 0x001b0898);

__attribute__((at(0x0005f620, "flashpatch")))
BPatch(flash_patch_93, 0x001b08e6);

__attribute__((at(0x000497dc, "flashpatch")))
BPatch(flash_patch_94, 0x001a74fa);

__attribute__((at(0x00032e98, "flashpatch")))
BPatch(flash_patch_95, 0x0019a172);

__attribute__((at(0x000332dc, "flashpatch")))
BPatch(flash_patch_96, 0x0018e434);

__attribute__((at(0x00033428, "flashpatch")))
BPatch(flash_patch_97, 0x00192370);

__attribute__((at(0x0003364c, "flashpatch")))
BPatch(flash_patch_98, 0x0019a136);

__attribute__((at(0x00033834, "flashpatch")))
BPatch(flash_patch_99, 0x0018dbc6);

__attribute__((at(0x0005c41c, "flashpatch")))
BPatch(flash_patch_100, 0x001addd4);

__attribute__((at(0x00033cf0, "flashpatch")))
BPatch(flash_patch_101, 0x0018dfde);

__attribute__((at(0x0005c958, "flashpatch")))
BPatch(flash_patch_102, 0x001aeb40);

__attribute__((at(0x00062118, "flashpatch")))
BPatch(flash_patch_103, 0x001b0c0a);

__attribute__((at(0x00034850, "flashpatch")))
BPatch(flash_patch_104, 0x0018bc5c);

__attribute__((at(0x0004a078, "flashpatch")))
BPatch(flash_patch_105, 0x001a928c);

__attribute__((at(0x0004a8b8, "flashpatch")))
BPatch(flash_patch_106, 0x001a90ec);

__attribute__((at(0x00062548, "flashpatch")))
BPatch(flash_patch_107, 0x001b0c5c);

__attribute__((at(0x0006bb1c, "flashpatch")))
BPatch(flash_patch_108, 0x001c74aa);

__attribute__((at(0x00063fc8, "flashpatch")))
BPatch(flash_patch_109, 0x001b11d6);

__attribute__((at(0x0006407c, "flashpatch")))
BPatch(flash_patch_110, 0x001b11a4);

__attribute__((at(0x0004b0c0, "flashpatch")))
BPatch(flash_patch_111, 0x001a859c);

__attribute__((at(0x00064270, "flashpatch")))
BPatch(flash_patch_112, 0x001b207e);

__attribute__((at(0x00064414, "flashpatch")))
BPatch(flash_patch_113, 0x001b17e2);

__attribute__((at(0x000657c8, "flashpatch")))
BPatch(flash_patch_114, 0x001b1928);

__attribute__((at(0x00066418, "flashpatch")))
BPatch(flash_patch_115, 0x001b21fc);

__attribute__((at(0x00066794, "flashpatch")))
BPatch(flash_patch_116, 0x001b2d76);

__attribute__((at(0x00066808, "flashpatch")))
BPatch(flash_patch_117, 0x001b2e38);

__attribute__((at(0x00066cb0, "flashpatch")))
BPatch(flash_patch_118, 0x001b2f12);

__attribute__((at(0x000690f4, "flashpatch")))
BPatch(flash_patch_119, 0x001b3640);

__attribute__((at(0x0006a404, "flashpatch")))
BPatch(flash_patch_120, 0x001b33ae);

__attribute__((at(0x0006a4b0, "flashpatch")))
BPatch(flash_patch_121, 0x001b348c);

__attribute__((at(0x0003561c, "flashpatch")))
BPatch(flash_patch_122, 0x0018b688);

__attribute__((at(0x000365a8, "flashpatch")))
BPatch(flash_patch_123, 0x0018af16);

__attribute__((at(0x00036650, "flashpatch")))
BPatch(flash_patch_124, 0x0019a162);

__attribute__((at(0x00036894, "flashpatch")))
BPatch(flash_patch_125, 0x0018ae08);

__attribute__((at(0x0006d8a4, "flashpatch")))
BPatch(flash_patch_126, 0x001c8372);

__attribute__((at(0x0006d988, "flashpatch")))
BPatch(flash_patch_127, 0x001c831a);

__attribute__((at(0x0006d9e8, "flashpatch")))
BPatch(flash_patch_128, 0x001c82ce);

__attribute__((at(0x0005cdf0, "flashpatch")))
BPatch(flash_patch_129, 0x001ad7c6);

__attribute__((at(0x00036c08, "flashpatch")))
BPatch(flash_patch_130, 0x0019132c);

__attribute__((at(0x0006e558, "flashpatch")))
BPatch(flash_patch_131, 0x001c8820);

__attribute__((at(0x0006e9a8, "flashpatch")))
BPatch(flash_patch_132, 0x001c8bc0);

__attribute__((at(0x0006f180, "flashpatch")))
BPatch(flash_patch_133, 0x001c88e4);

__attribute__((at(0x0006e39c, "flashpatch")))
BPatch(flash_patch_134, 0x001c8540);

__attribute__((at(0x0003753c, "flashpatch")))
BPatch(flash_patch_135, 0x00193958);

__attribute__((at(0x00042ab4, "flashpatch")))
BPatch(flash_patch_136, 0x001a3e8e);

__attribute__((at(0x0004b9c0, "flashpatch")))
BPatch(flash_patch_137, 0x001aa03a);

__attribute__((at(0x000726ac, "flashpatch")))
BPatch(flash_patch_138, 0x001c8dd0);

__attribute__((at(0x00074298, "flashpatch")))
BPatch(flash_patch_139, 0x001c8e16);

__attribute__((at(0x0007557c, "flashpatch")))
BPatch(flash_patch_140, 0x001c99bc);

__attribute__((at(0x00075914, "flashpatch")))
BPatch(flash_patch_141, 0x001c9f4c);

__attribute__((at(0x00037a90, "flashpatch")))
BPatch(flash_patch_142, 0x00191440);

__attribute__((at(0x00075b10, "flashpatch")))
BPatch(flash_patch_143, 0x001c98d8);

__attribute__((at(0x0007604c, "flashpatch")))
BPatch(flash_patch_144, 0x001ca0ec);

__attribute__((at(0x00076344, "flashpatch")))
BPatch(flash_patch_145, 0x001ca05c);

__attribute__((at(0x00076660, "flashpatch")))
BPatch(flash_patch_146, 0x001ca404);

__attribute__((at(0x00037ba8, "flashpatch")))
BPatch(flash_patch_147, 0x0018ae98);

__attribute__((at(0x0007724c, "flashpatch")))
BPatch(flash_patch_148, 0x001ca828);

__attribute__((at(0x000774f4, "flashpatch")))
BPatch(flash_patch_149, 0x001caacc);

__attribute__((at(0x00077704, "flashpatch")))
BPatch(flash_patch_150, 0x001cace8);

__attribute__((at(0x00077744, "flashpatch")))
BPatch(flash_patch_151, 0x001cad1a);

__attribute__((at(0x000767b8, "flashpatch")))
BPatch(flash_patch_152, 0x001ca610);

__attribute__((at(0x00076ac8, "flashpatch")))
BPatch(flash_patch_153, 0x001ca3c4);

__attribute__((at(0x0005f910, "flashpatch")))
BPatch(flash_patch_154, 0x001b033c);

__attribute__((at(0x00038e40, "flashpatch")))
BPatch(flash_patch_155, 0x001934a8);

__attribute__((at(0x00038eb0, "flashpatch")))
BPatch(flash_patch_156, 0x00197568);

__attribute__((at(0x00039988, "flashpatch")))
BPatch(flash_patch_157, 0x00190d10);

__attribute__((at(0x0005d1a4, "flashpatch")))
BPatch(flash_patch_158, 0x001aeace);

__attribute__((at(0x00039f9c, "flashpatch")))
BPatch(flash_patch_159, 0x001907a0);

__attribute__((at(0x0005d260, "flashpatch")))
BPatch(flash_patch_160, 0x001ad8fc);

__attribute__((at(0x0003a494, "flashpatch")))
BPatch(flash_patch_161, 0x00190ece);

__attribute__((at(0x0003a95c, "flashpatch")))
BPatch(flash_patch_162, 0x0018afb4);

__attribute__((at(0x0003af7c, "flashpatch")))
BPatch(flash_patch_163, 0x0018ad36);

__attribute__((at(0x0007b374, "flashpatch")))
BPatch(flash_patch_164, 0x001cd04a);

__attribute__((at(0x0007c24c, "flashpatch")))
BPatch(flash_patch_165, 0x001cffe0);

__attribute__((at(0x0007cab0, "flashpatch")))
BPatch(flash_patch_166, 0x001cf190);

__attribute__((at(0x0007d5e4, "flashpatch")))
BPatch(flash_patch_167, 0x001cdc84);

__attribute__((at(0x0007db64, "flashpatch")))
BPatch(flash_patch_168, 0x001cfbbc);

__attribute__((at(0x0007defc, "flashpatch")))
BPatch(flash_patch_169, 0x001cec92);

__attribute__((at(0x0007e2c4, "flashpatch")))
BPatch(flash_patch_170, 0x001ceb82);

__attribute__((at(0x0007e41c, "flashpatch")))
BPatch(flash_patch_171, 0x001cf746);

__attribute__((at(0x0007e644, "flashpatch")))
BPatch(flash_patch_172, 0x001cf248);

__attribute__((at(0x0007e90c, "flashpatch")))
BPatch(flash_patch_173, 0x001cf4f4);

__attribute__((at(0x0007eb98, "flashpatch")))
BPatch(flash_patch_174, 0x001cf9b8);

__attribute__((at(0x0007eefc, "flashpatch")))
BPatch(flash_patch_175, 0x001ce340);

__attribute__((at(0x0007f13c, "flashpatch")))
BPatch(flash_patch_176, 0x001cdc06);

__attribute__((at(0x0007f3e8, "flashpatch")))
BPatch(flash_patch_177, 0x001cdcbc);

__attribute__((at(0x0007f66c, "flashpatch")))
BPatch(flash_patch_178, 0x001ce728);

__attribute__((at(0x00080004, "flashpatch")))
BPatch(flash_patch_179, 0x001cfe30);

__attribute__((at(0x00080354, "flashpatch")))
BPatch(flash_patch_180, 0x001cd16e);

__attribute__((at(0x000811a0, "flashpatch")))
BPatch(flash_patch_181, 0x001d0194);

__attribute__((at(0x000814f8, "flashpatch")))
BPatch(flash_patch_182, 0x001cefb8);

__attribute__((at(0x00081e48, "flashpatch")))
BPatch(flash_patch_183, 0x001d03c8);

__attribute__((at(0x0003b2b8, "flashpatch")))
BPatch(flash_patch_184, 0x00199f04);

__attribute__((at(0x0003b5f0, "flashpatch")))
BPatch(flash_patch_185, 0x0018c034);

__attribute__((at(0x0003b6c8, "flashpatch")))
BPatch(flash_patch_186, 0x0018e9d6);

__attribute__((at(0x0003b768, "flashpatch")))
BPatch(flash_patch_187, 0x0018ea32);

__attribute__((at(0x0003b858, "flashpatch")))
BPatch(flash_patch_188, 0x00199e64);

__attribute__((at(0x0003b92c, "flashpatch")))
BPatch(flash_patch_189, 0x0019f0ec);

__attribute__((at(0x0003ba08, "flashpatch")))
BPatch(flash_patch_190, 0x001907ea);

__attribute__((at(0x000823b0, "flashpatch")))
BPatch(flash_patch_191, 0x001d1154);

__attribute__((at(0x000828fc, "flashpatch")))
BPatch(flash_patch_192, 0x001d0fe4);

__attribute__((at(0x00082fac, "flashpatch")))
BPatch(flash_patch_193, 0x001d1480);

__attribute__((at(0x0003bc40, "flashpatch")))
BPatch(flash_patch_194, 0x00199e10);

__attribute__((at(0x0003bc6c, "flashpatch")))
BPatch(flash_patch_195, 0x00199e36);

__attribute__((at(0x0006b930, "flashpatch")))
BPatch(flash_patch_196, 0x001b3734);

__attribute__((at(0x000276e4, "flashpatch")))
BPatch(flash_patch_197, 0x00189700);
