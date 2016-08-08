#include "../../include/bcm4339.h"
#include "../../include/debug.h"
#include "../../include/wrapper.h"
#include "../../include/structs.h"
#include "../../include/helper.h"

/**
 *	A simple hello world printing function
 */
void
hello_world(void)
{
	printf("hello world!\n");			// Calls a function in ROM. Its location is defined in wrapper.h
}

/**
 *	is called before the wlc_ucode_download function to print our hello world message
 */
__attribute__((naked)) void
wlc_ucode_download_hook(void)
{
	asm(
		"push {r0-r3,lr}\n"				// Push the registers that could be modified by a call to a C function
		"bl hello_world\n"				// Call a C function
		"pop {r0-r3,lr}\n"				// Pop the registers that were saved before
		"b wlc_ucode_download\n"		// Call the hooked function
		);
}

/**
 *	Just inserted to produce an error while linking, when we try to overwrite existing memory
 */
void 
dummy_180800(void)
{
	;
}