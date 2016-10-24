#!/bin/bash
printf "#include \"../include/firmware_version.h\"\n%s\n" $1 | gcc -E -x c - | tail -n 1