mapaddr : mapaddr.c
	gcc -o mapaddr mapaddr.c

trace.txt : trace.bin
	xxd -c 4 trace.bin | awk '{print $$2 $$3}' > trace.txt
