#ifndef HELPER_H
#define HELPER_H

inline void *
get_stack_ptr() {
    void *stack = 0x0;
    __asm("mov %0, sp" : "=r" (stack));
    return stack;
}

#endif /* HELPER_H */
