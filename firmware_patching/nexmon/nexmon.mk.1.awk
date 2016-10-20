{
	if ($2 == "TARGETREGION") {
		"./getsectionaddr.sh .text." $1 " patch.elf" | getline result; print $0 " " result
	} else {
		print;
	}; 
}