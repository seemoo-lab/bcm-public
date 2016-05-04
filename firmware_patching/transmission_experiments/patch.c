/***************************************************************************
 *                                                                         *
 *          ###########   ###########   ##########    ##########           *
 *         ############  ############  ############  ############          *
 *         ##            ##            ##   ##   ##  ##        ##          *
 *         ##            ##            ##   ##   ##  ##        ##          *
 *         ###########   ####  ######  ##   ##   ##  ##    ######          *
 *          ###########  ####  #       ##   ##   ##  ##    #    #          *
 *                   ##  ##    ######  ##   ##   ##  ##    #    #          *
 *                   ##  ##    #       ##   ##   ##  ##    #    #          *
 *         ############  ##### ######  ##   ##   ##  ##### ######          *
 *         ###########    ###########  ##   ##   ##   ##########           *
 *                                                                         *
 *            S E C U R E   M O B I L E   N E T W O R K I N G              *
 *                                                                         *
 * Warning:                                                                *
 *                                                                         *
 * Our software may damage your hardware and may void your hardware’s      *
 * warranty! You use our tools at your own risk and responsibility!        *
 *                                                                         *
 * License:                                                                *
 * Copyright (c) 2015 NexMon Team                                          *
 *                                                                         *
 * Permission is hereby granted, free of charge, to any person obtaining   *
 * a copy of this software and associated documentation files (the         *
 * "Software"), to deal in the Software without restriction, including     *
 * without limitation the rights to use, copy, modify, merge, publish,     *
 * distribute copies of the Software, and to permit persons to whom the    *
 * Software is furnished to do so, subject to the following conditions:    *
 *                                                                         *
 * The above copyright notice and this permission notice shall be included *
 * in all copies or substantial portions of the Software.                  *
 *                                                                         *
 * Any use of the Software which results in an academic publication or     *
 * other publication which includes a bibliography must include a citation *
 * to the author's publication "M. Schulz, D. Wegemer and M. Hollick.      *
 * NexMon: A Cookbook for Firmware Modifications on Smartphones to Enable  *
 * Monitor Mode.".                                                         *
 *                                                                         *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF              *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY    *
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,    *
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE       *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                  *
 *                                                                         *                                                       *
 **************************************************************************/

#include "../include/bcm4339.h"	// contains addresses specific for BCM4339
#include "../include/debug.h"	// contains macros to access the debug hardware
#include "../include/wrapper.h"	// wrapper definitions for functions that already exist in the firmware
#include "../include/structs.h"	// structures that are used by the code in the firmware
#include "../include/helper.h"	// useful helper functions

/**
 *	Saves one-hot encoded which breakpoint was hit
 */
unsigned char breakpoint_hit = 0;

unsigned int breakpoint_cnt1 = 0;
unsigned int breakpoint_cnt2 = 0;

__attribute__((naked)) void
enable_interrupts_and_wait_hook(void)
{
	asm(
		"push {r0-r3,lr}\n"
		"bl enable_interrupts_and_wait_hook_in_c\n"
		"pop {r0-r3,lr}\n"
		"b enable_interrupts_and_wait\n"
		);
}

char *pkt = "\xaa\xaa\xaa\xaa\xaa\xaa\xbb\xbb\xbb\xbb\xbb\xbb\xbb\x08\x00""AAAAAAAA";

void
enable_interrupts_and_wait_hook_in_c(void)
{
	sk_buff *p;
	int ret = 0;
	struct wlc_info *wlc = (struct wlc_info *) 0x1e8d7c;
	char *bsscfg;
	char *bsscfg_0x30C;
	int *scb;

	printf("test\n");
	p = pkt_buf_get_skb(wlc->osh, 22);

	memcpy(p->data,pkt,22);

	bsscfg = (char *) wlc_bsscfg_find_by_wlcif(wlc, 0);
	bsscfg_0x30C = *(char **) (bsscfg+0x30c);

	bsscfg[0x04] = 1;
	bsscfg[0x06] = 1;
	
	scb = __wlc_scb_lookup(wlc, bsscfg, pkt, 0);
	wlc_scb_set_bsscfg(scb, bsscfg);
	//scb[60] = wlc->active_queue;
	printf("scb10=%08x ac=%08x\n", scb[4], wlc->active_queue); 

	printf("D %08x %08x %08x %08x\n", (int) bsscfg, *(int *) bsscfg, *(int *) (bsscfg+4), *(int *) (bsscfg_0x30C + 50));

	ret = wlc_sendpkt(wlc, p, 0);
	
	bsscfg[0x04] = 0;
	bsscfg[0x06] = 0;

	printf("test %d\n", ret);	
}

/**
 *	Replaces the SP and LR registers on the stack with those of the system mode.
 *	As we decided to stay in abort mode to handle breakpoints and watchpoints, we need to 
 *	change to system mode to fetch the correct sp and lr registers to store the correct state
 *	on the stack. The required state is the one before the prefetch/data abort exception.
 */
struct trace * __attribute__((optimize("O0")))
fix_sp_lr(struct trace *trace)
{
	register unsigned int sp_sys asm("r1");
	register unsigned int lr_sys asm("r2");

	dbg_disable_monitor_mode_debugging();
	dbg_change_processor_mode(DBG_PROCESSOR_MODE_SYS);
	asm("mov %[result], sp" : [result] "=r" (sp_sys));
	asm("mov %[result], lr" : [result] "=r" (lr_sys));
	dbg_change_processor_mode(DBG_PROCESSOR_MODE_ABT);
	dbg_enable_monitor_mode_debugging();

	trace->lr = lr_sys;
	trace->sp = sp_sys;

	return trace;
}

void __attribute__((optimize("O0")))
handle_pref_abort_exception(struct trace *trace)
{
	fix_sp_lr(trace);
	
	if(dbg_is_breakpoint_enabled(0)) {
		if (dbg_triggers_on_breakpoint_address(0, trace->pc)) {
			// to continue executed on the instruction where breakpoint 0 triggered, we set the breakpoint type to address mismatch to trigger on any instruction except the breakpoint address
			dbg_set_breakpoint_type_to_instr_addr_mismatch(0);

			printf("hit\n");

			// to know which breakpoint mismatch was triggerd on a next breakpoint hit, we set a bit in the breakpoint_hit variable
			breakpoint_hit |= DBGBP0;
		} else if (breakpoint_hit & DBGBP0) {
			// we reset the the breakpoint for address matching
			//dbg_set_breakpoint_type_to_instr_addr_match(0);

			dbg_set_breakpoint_for_addr_mismatch(0, trace->pc);

			if ((trace->pc > 0x197A18) && (trace->pc < 0x197E34)) {
				printf("A%d/%d:%08x\n", breakpoint_cnt1, breakpoint_cnt2, trace->pc);
				breakpoint_cnt2++;
			} else {
				printf("");
				//printf("B%d/%d:%08x\n", breakpoint_cnt1, breakpoint_cnt2, trace->pc);
			}
			breakpoint_cnt1++;

			if ((breakpoint_cnt1 > 1500) || (breakpoint_cnt2 > 150) || (trace->pc == 0x197e2c)) {
				printf("disable\n");
				printf("0=%08x 1=%08x 2=%08x 3=%08x 6=%08x\n", trace->r0, trace->r1, trace->r2, trace->r3, trace->r6);
				dbg_disable_breakpoint(0);
			}

			// we set the bit in the breakpoint_hit variable to 0
			//breakpoint_hit &= ~DBGBP0;
		} else {
			printf("ERR PC=%08x\n", trace->pc);
		}
	} else {
		printf("ERR PC=%08x\n", trace->pc);
	}
}

/**
 *	Sets the stack pointer of the abort mode to be at the end of the current stack area, a part that is likely not used during regular program execution
 */
void __attribute__((optimize("O0")))
set_abort_stack_pointer(void)
{
	register unsigned int sp_abt asm("r0") = 0x23DAD0;

	dbg_change_processor_mode(DBG_PROCESSOR_MODE_ABT);
	asm("mov sp, %[value]" : : [value] "r" (sp_abt));
	dbg_change_processor_mode(DBG_PROCESSOR_MODE_SYS);
}

void
set_debug_registers(void)
{
	set_abort_stack_pointer();
	dbg_unlock_debug_registers();
	dbg_disable_breakpoint(0);
	dbg_disable_breakpoint(1);
	dbg_disable_breakpoint(2);
	dbg_disable_breakpoint(3);
	dbg_enable_monitor_mode_debugging();
	
	// Programm Breakpoint to match the instruction we want to hit
	//dbg_set_breakpoint_for_addr_match(0, 0x1ecab0);
	//dbg_set_breakpoint_for_addr_match(0, 0x1AAD98); // wlc_bmac_recv
	//dbg_set_breakpoint_for_addr_match(0, 0x19551C); // wlc_ioctl
	//dbg_set_breakpoint_for_addr_match(0, 0x1844B2); // dma_txfast
	dbg_set_breakpoint_for_addr_match(0, (int) wlc_sendpkt);
}

/**
 *	is called before the wlc_ucode_download function to print our hello world message
 */
__attribute__((naked)) void
before_before_initialize_memory_hook(void)
{
	asm(
		"push {r0-r3,lr}\n"							// Push the registers that could be modified by a call to a C function
		"bl set_debug_registers\n"					// Call a C function
		"pop {r0-r3,lr}\n"							// Pop the registers that were saved before
		"b before_before_initialize_memory\n"		// Call the hooked function
		);
}

/**
 *	Exception handler that is triggered on reset/startup/powerup 
 */
__attribute__((naked)) void
tr_reset_hook(void)
{
	asm(
		"mrs r0, cpsr\n"
		"mov r1, #0x1f\n"
		"bic r0, r0, r1\n"
		"mov r1, #0xdf\n"
		"orr r0, r0, r1\n"
		"msr cpsr_fc, r0\n"
		"b setup\n"
		);
}


/**
 *	Exception handler that is triggered by breakpoint 
 */
__attribute__((naked)) void
tr_pref_abort_hook(void)
{
	asm(
		"sub lr, lr, #4\n"									// we directly subtract 4 from the link register (original code overwrites the stack pointer first)
		"srsdb sp!, #0x17\n"								// stores return state (LR and SPSR) to the stack of the abort mode (0x17) (original codes switches to system mode (0x1F))
		"push {r0}\n"
		"push {lr}\n"										// here we do not get the expected link register from the system mode, but the link register from the abort mode
		"sub sp, sp, #24\n"
		"push {r0-r7}\n"
		"eor r0, r0, r0\n"
		"add r0, r0, #3\n"
		"b handle_exceptions\n"
		);
}

__attribute__((naked)) void
choose_exception_handler(void)
{
	asm(
			"cmp r0, #3\n"
			"beq label_pref_abort\n"
		"label_continue_exception:\n"
			"cmp r0, #6\n"									// check if fast interrupt
			"bne label_other_exception\n"					// if interrupt was not a fast interrupt
			"cmp r1, #64\n"									// if FIQ was enabled, enable it again
			"beq label_other_exception\n"
			"cpsie f\n"
		"label_other_exception:\n"
			"mov r0, sp\n"
			"push {lr}\n"
			"pop {lr}\n"
			"b dump_stack_print_dbg_stuff_intr_handler\n"
		"label_pref_abort:\n"
			"mov r0, sp\n"
			"push {lr}\n"
			"pop {lr}\n"
			"b handle_pref_abort_exception\n"
		);
}

/**
 *
 */
__attribute__((naked)) void
handle_exceptions(void)
{
	asm(
		"mov r4, sp\n"
		"add r4, r4, #64\n"
		"ldmia r4!, {r1,r3}\n"
		"mrs r2, cpsr\n"
		"push {r0-r3}\n"
		"sub r4, r4, #12\n"
		"str r1, [r4]\n"
		"and r1, r3, #64\n"				// r3 is CPSR_SYS, bit 6 is the FIQ mask bit
		"mov r7, sp\n"
		"add r7, r7, #88\n"
		"mov r6, r12\n"
		"mov r5, r11\n"
		"mov r4, r10\n"
		"mov r3, r9\n"
		"mov r2, r8\n"
		"add sp, sp, #72\n"
		"push {r2-r7}\n"
		/*
		 * Now the stack looks like this:
		 * SPSR -> CPSR_SYS
		 * LR_ABT -> PC_SYS
		 * PC_SYS
		 * LR_ABT call fix_sp_lr to change this to LR_SYS
		 * SP_ABT call fix_sp_lr to change this to SP_SYS
		 * R12
		 * R11
		 * R10
		 * R9
		 * R8
		 * R7
		 * R6
		 * R5
		 * R4
		 * R3
		 * R2
		 * R1
		 * R0
		 * CPSR_SYS
		 * CPSR_ABT
		 * PC_SYS
		 * Exception ID
		 */
		"sub sp, sp, #48\n"
		"bl choose_exception_handler\n"
		"cpsid if\n"
		"add sp, sp, #48\n"
		"pop {r0-r6}\n"
		"mov r8, r0\n"
		"mov r9, r1\n"
		"mov r10, r2\n"
		"mov r11, r3\n"
		"mov r12, r4\n"
		"mov lr, r6\n"
		"sub sp, sp, #60\n"
		"pop {r0-r7}\n"
		"add sp, sp, #32\n"
		"rfefd sp!\n"
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

/**
 *	Just inserted to produce an error while linking, when we try to overwrite existing memory
 */
void 
dummy_181098(void)
{
	;
}