ucodeext : ucodeext.o darm/armv7.o darm/armv7-tbl.o darm/darm.o darm/darm-tbl.o darm/thumb2.o darm/thumb2-decoder.o darm/thumb2-tbl.o darm/thumb.o darm/thumb-tbl.o
	gcc -o $@ $^

ucodeext.o : ucodeext.c
	gcc -std=c99 -Wall -Wno-unused-result -O0 -D_BSD_SOURCE -o $@ -c $<

darm/%.o: darm/%.c
	gcc -std=c99 -Wall -O2 -Wextra -Wno-missing-field-initializers -o $@ -c $^
