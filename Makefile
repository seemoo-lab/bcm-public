all: tools

tools: bin/mkbootimg bin/unmkbootimg bin/mkbootfs

bin/mkbootimg: src/mkbootimg/mkbootimg.c
	cd src/ && \
	   gcc -std=c99 -o ../bin/mkbootimg libmincrypt/*.c mkbootimg/mkbootimg.c -Iinclude

bin/unmkbootimg: src/mkbootimg/unmkbootimg.c
	cd src/ && \
	   gcc -std=c99 -o ../bin/unmkbootimg libmincrypt/*.c mkbootimg/unmkbootimg.c -Iinclude

bin/mkbootfs: src/cpio/mkbootfs.c
	cd src/ && \
	   gcc -o ../bin/mkbootfs cpio/mkbootfs.c libcutils/fs_config.c -Iinclude

clean:
	rm -f bin/*
