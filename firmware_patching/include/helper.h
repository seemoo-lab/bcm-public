#ifndef HELPER_H
#define HELPER_H

#include "bcm4339.h"

inline void *
get_stack_ptr() {
    void *stack = 0x0;
    __asm("mov %0, sp" : "=r" (stack));
    return stack;
}

inline void
copy_stack(void *dest, int copy_size) {
    printf("copy_stack: %d\n", copy_size);
    if(copy_size > 0) {
        memcpy( (void *) (dest), get_stack_ptr(), copy_size);
    }   
    return;
}

inline void
hexdump(char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != 0)
        printf ("%s:\n", desc);

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}

#endif /* HELPER_H */
