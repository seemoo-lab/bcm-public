{
	if ($2 == "PATCH")
		print "\t$(Q)$(CC)objcopy -O binary -j .text." $4 " $< section.generated.bin && dd if=section.generated.bin of=$@ bs=1 conv=notrunc status=none seek=$$((" $1 " - " ramstart "))\n\t$(Q)printf \"  PATCH " $4 " @ " $1 "\\n\"";
	else if ($2 == "REGION")
		print "\t$(Q)$(CC)objcopy -O binary -j .text." $1 " $< section.generated.bin && dd if=section.generated.bin of=$@ bs=1 conv=notrunc status=none seek=$$((0x" $4 " - " ramstart "))\t$(Q)printf \"  REGION " $1 " @ " $4 "\\n\"";
	else if ($2 == "TARGETREGION" && $4 != "")
		print "\t$(Q)$(CC)objcopy -O binary -j .text." $1 " $< section.generated.bin && dd if=section.generated.bin of=$@ bs=1 conv=notrunc status=none seek=$$((0x" $4 " - " ramstart "))\n\t$(Q)printf \"  TARGETREGION " $1 " @ " $4 "\\n\"";
}
