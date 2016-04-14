#include "../include/bcm4339.h"
#include "../include/debug.h"
#include "../include/wrapper.h"
#include "../include/structs.h"
#include "../include/helper.h"
#include "../include/types.h" /* needs to be included before bcmsdpcm.h */
#include "../include/bcmdhd/bcmsdpcm.h"
#include "../include/bcmdhd/bcmcdc.h"

#include "bdc_ethernet_ipv6_udp_header_array.h"

#define SDIO_SEQ_NUM 0x108

extern void *dngl_sendpkt_alternative(void *sdio, void *p, int chan);
struct sk_buff *create_frame(unsigned int hooked_fct, unsigned int arg0, unsigned int arg1, unsigned int arg2, void *start_address, unsigned int length);

void
try_to_access_d11(void)
{
	printf("%08x: %08x\n", 0x1800101C, *(int *) 0x1800101C);
	//printf("%08x: %08x\n", 0x1810101C, *(int *) 0x1810101C);
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

/**
 *	Exception handler that is triggered by watchpoint or illegal address access 
 */
__attribute__((naked)) void
tr_data_abort_hook(void)
{
	asm(
		"sub lr, lr, #8\n"									// we directly subtract 4 from the link register (original code overwrites the stack pointer first)
		"srsdb sp!, #0x17\n"								// stores return state (LR and SPSR) to the stack of the abort mode (0x17) (original codes switches to system mode (0x1F))
		"push {r0}\n"
		"push {lr}\n"										// here we do not get the expected link register from the system mode, but the link register from the abort mode
		"sub sp, sp, #24\n"
		"push {r0-r7}\n"
		"eor r0, r0, r0\n"
		"add r0, r0, #4\n"
		"b handle_exceptions\n"
		);
}


int ext_intr_counter = 0;

void
dbgout(int * sp)
{
	asm("push {r0-r3}");
	if (ext_intr_counter < 20)
		printf("dbg %d %08x %d\n", *sp, *(sp+1), ext_intr_counter++);
//	printf("dbg %08x\n", *(volatile int *) 0x18007000);
	asm("pop {r0-r3}");
}


__attribute__((naked)) void
choose_exception_handler(void)
{
	asm(
			"cmp r0, #3\n"
			"beq label_pref_abort\n"
			"cmp r0, #4\n"
			"beq label_data_abort\n"
		"label_continue_exception:\n"
			"cmp r0, #6\n"									// check if fast interrupt
			"bne label_other_exception\n"					// if interrupt was not a fast interrupt
			"cmp r1, #64\n"									// if FIQ was enabled, enable it again
			"beq label_other_exception\n"
			"cpsie f\n"
		"label_other_exception:\n"
			"mov r0, sp\n"
			"push {lr}\n"
			//"bl dbgout\n"
			"pop {lr}\n"
			"b dump_stack_print_dbg_stuff_intr_handler\n"
		"label_pref_abort:\n"
			"mov r0, sp\n"
			"push {lr}\n"
			//"bl dbgout\n"
			"pop {lr}\n"
			"b handle_pref_abort_exception\n"
		"label_data_abort:\n"
			"mrc p15, 0, r0, c5, c0, 0\n"					// read DFSR
			"and r0, r0, #0x1F\n"							// select FS bits from DFSR
			"cmp r0, #2\n"									// compare FS to "debug event"
			"bne label_continue_exception\n"				// if data abort not caused by a "debug event", go to regular exception handler
			"mov r0, sp\n"
			"b handle_data_abort_exception\n"
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
 *	Enables IRQ interrupt in CPSR
 */
__attribute__((naked)) void
enable_irq_interrupt(void)
{
	asm(
		"cpsie i\n"
		"bx lr\n"
		);
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

/**
 *	Saves one-hot encoded which breakpoint was hit
 */
unsigned char breakpoint_hit = 0;

/**
 *	Breakpoint hit counter, saves how often a breakpoint was hit
 */
unsigned char breakpoint_hit_counter[DBG_NUMBER_OF_BREAKPOINTS] = { 0, 0, 0, 0 };

/**
 *	Breakpoint hit limit, defines how often a breakpoint should trigger
 */
unsigned char breakpoint_hit_limit[DBG_NUMBER_OF_BREAKPOINTS] = { 130, 100, 2, 1 };

/**
 *	Saves one-hot encoded which watchpoint was hit
 */
unsigned char watchpoint_hit = 0;

/**
 *	Watchpoint hit counter, saves how often a watchpoint was hit
 */
unsigned char watchpoint_hit_counter = 0;

/**
 *	Watchpoint hit limit, defines how often a watchpoint should trigger
 */
unsigned char watchpoint_hit_limit = 1;

void
print_breakpoint_infos(int breakpoint_number, struct trace *trace, int dump_size)
{
	switch(dump_size) {
		case 0:
			printf("BP%d(%d) PC=%08x LR=%08x 0=%08x 1=%08x 1=%08x 3=%08x 4=%08x 5=%08x 6=%08x\n", 
				breakpoint_number, 
				breakpoint_hit_counter[breakpoint_number], 
				trace->PC, 
				trace->lr, 
				trace->r0, 
				trace->r1, 
				trace->r2, 
				trace->r3, 
				trace->r4, 
				trace->r5, 
				trace->r6);
			break;
		case 1:
			printf("BP%d(%d) PC=%08x LR=%08x\n", 
				breakpoint_number, 
				breakpoint_hit_counter[breakpoint_number], 
				trace->PC, 
				trace->lr);
			break;
	}
}

unsigned int cnt[6] = {0};

void
handle_pref_abort_exception(struct trace *trace)
{
	fix_sp_lr(trace);
	//printf("BP(%08x): %08x %08x %08x\n", trace->PC, trace->sp, trace->lr, breakpoint_hit);
	
	if(dbg_is_breakpoint_enabled(0)) {
		if (dbg_triggers_on_breakpoint_address(0, trace->pc)) {
			// to continue executed on the instruction where breakpoint 0 triggered, we set the breakpoint type to address mismatch to trigger on any instruction except the breakpoint address
			dbg_set_breakpoint_type_to_instr_addr_mismatch(0);
			// to know which breakpoint mismatch was triggerd on a next breakpoint hit, we set a bit in the breakpoint_hit variable
			breakpoint_hit |= DBGBP0;
			breakpoint_hit_counter[0]++;
			//dbg_set_watchpoint_for_addr_match_with_mask(0, 0x18001000, DBGWCR_MASK_00000FFF);
			//dbg_set_watchpoint_for_addr_match_with_mask(1, 0x18101000, DBGWCR_MASK_00000FFF);
			//if (trace->r0 != 0x1eab98) {
			//if (breakpoint_hit_counter[0] == 7) {
			//	int i = 0;
			//	printf("di:%08x\n", trace->r0);
			//	for (i = 0; i < 52; i++)
			//		printf("%08x\n", *((int *) trace->r0 + i));
			//}
			switch(trace->r0) {
				case 0x1eab98:
				cnt[0]++;
				break;
				case 0x1e4288:
				cnt[1]++;
				break;
				case 0x1e3a04:
				cnt[2]++;
				break;
				case 0x1e3414:
				cnt[3]++;
				break;
				case 0x1e2e24:
				cnt[4]++;
				break;
				case 0x1e2834:
				cnt[5]++;
				break;
			}
			if (cnt[0] % 10 == 1)
				printf("0:%d 1:%d 2:%d 3:%d 4:%d 5:%d\n", cnt[0], cnt[1], cnt[2], cnt[3], cnt[4], cnt[5]);
			//print_breakpoint_infos(0, trace, 0);
		} else if (breakpoint_hit & DBGBP0) {
			// we reset the the breakpoint for address matching
			dbg_set_breakpoint_type_to_instr_addr_match(0);
			if (0 && breakpoint_hit_counter[0] == breakpoint_hit_limit[0]) {
			//if (trace->r0 != 0x1eab98) {
				dbg_disable_breakpoint(0);
			}
			// we set the bit in the breakpoint_hit variable to 0
			breakpoint_hit &= ~DBGBP0;
			//print_breakpoint_infos(0, trace, 1);
		}
	} else if (dbg_is_breakpoint_enabled(1)) {
		if (dbg_triggers_on_breakpoint_address(1, trace->pc)) {
			dbg_set_breakpoint_type_to_instr_addr_mismatch(1);
			breakpoint_hit |= DBGBP1;
			breakpoint_hit_counter[1]++;
			if(trace->r3 == 0x27551) {
				print_breakpoint_infos(1, trace, 0);
				//try_to_access_d11();
			}
			//try_to_access_d11();
		} else if (breakpoint_hit & DBGBP1) {
			dbg_set_breakpoint_type_to_instr_addr_match(1);
			if (breakpoint_hit_counter[1] == breakpoint_hit_limit[1]) {
				dbg_disable_breakpoint(1);
			}
			breakpoint_hit &= ~DBGBP1;
			print_breakpoint_infos(1, trace, 1);
		}
	} else if (dbg_is_breakpoint_enabled(2)) {
		if (dbg_triggers_on_breakpoint_address(2, trace->pc)) {
			dbg_set_breakpoint_type_to_instr_addr_mismatch(2);
			breakpoint_hit |= DBGBP2;
			breakpoint_hit_counter[2]++;
			print_breakpoint_infos(2, trace, 0);
		} else if (breakpoint_hit & DBGBP2) {
			dbg_set_breakpoint_type_to_instr_addr_match(2);
			if (breakpoint_hit_counter[2] == breakpoint_hit_limit[2]) {
				dbg_disable_breakpoint(2);
			}
			breakpoint_hit &= ~DBGBP2;
			print_breakpoint_infos(2, trace, 1);
		}
	} else if (dbg_is_breakpoint_enabled(3)) {
		// Used to reset watchpoint
		if (watchpoint_hit & DBGWP0) {
			dbg_disable_breakpoint(3);
			watchpoint_hit &= ~DBGWP0;
			if (++watchpoint_hit_counter < watchpoint_hit_limit) {
				dbg_enable_watchpoint(0);
				dbg_enable_watchpoint(1);
				//dbg_enable_watchpoint(2);
				//dbg_enable_watchpoint(3);
				printf("WP0 reset (%d)\n", watchpoint_hit_counter);
			} else {
				printf("WP0 not reset (%d)\n", watchpoint_hit_counter);
			}
		}
	}
}

unsigned int
read_dfar()
{
	unsigned int dfar;

	asm("mrc p15, 0, %[result], c6, c0, 0" : [result] "=r" (dfar));

	return dfar;
}

unsigned int
read_dfsr()
{
	unsigned int dfsr;

	asm("mrc p15, 0, %[result], c5, c0, 0" : [result] "=r" (dfsr));

	return dfsr;
}

void
handle_data_abort_exception(struct trace *trace)
{
	fix_sp_lr(trace);
	printf("WP(%08x): LR=%08x %08x %08x %08x %08x %08x\n", trace->PC, trace->lr, trace->r0, trace->r1, trace->r2, trace->r3, trace->r4);

	// TODO
	// Currently I do not know how to find out, which watchpoint was triggered, so here I always handle watchpoint 0
	watchpoint_hit |= DBGWP0;
	// Disable the watchpoint so that the instruction that triggered the watchpoint can be executed without triggering the watchpoint again.
	dbg_disable_watchpoint(0);
	dbg_disable_watchpoint(1);
	dbg_disable_watchpoint(2);
	dbg_disable_watchpoint(3);
	// Set breakpoint 3 to reset the watchpoint after executing the instruction that triggered the watchpoint.
	dbg_set_breakpoint_for_addr_mismatch(3, trace->PC);
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
	//dbg_set_breakpoint_for_addr_match(0, 0x1f59aa); // d11 access before sub_5054C
	//dbg_set_breakpoint_for_addr_match(0, 0x50554); // d11 access before sub_60DA4
	//dbg_set_breakpoint_for_addr_match(0, 0x50558); // d11 access after sub_60DA4
	//dbg_set_breakpoint_for_addr_match(0, 0x50570); // d11 access after sub_8C0FC
	//dbg_set_breakpoint_for_addr_match(0, 0x5057C); // d11 access after sub_8BE1C
	//dbg_set_breakpoint_for_addr_match(0, 0x50584); // d11 access after sub_4EB04
	//dbg_set_breakpoint_for_addr_match(0, 0x50594); // no breakpoint hit before sub_1D848
	//dbg_set_breakpoint_for_addr_match(0, 0x505A2); // d11 access before sub_1D444
	//dbg_set_breakpoint_for_addr_match(0, 0xfcb2); // d11 access
	//dbg_set_breakpoint_for_addr_match(0, 0x505A6); // no d11 access after sub_1D444
	//dbg_set_breakpoint_for_addr_match(0, 0x1f59ae); // no d11 access after sub_5054C
	//dbg_set_breakpoint_for_addr_match(0, 0x1f59b8); // no d11 access
	//dbg_set_breakpoint_for_addr_match(3, 0x1F31A2);
	//dbg_set_breakpoint_for_addr_match(1, 0x181a88); // no d11 access in external_interrupt handler
	//dbg_set_breakpoint_for_addr_match(1, 0x181aa8); // no d11 access after call of interrupt handlers
//	dbg_set_breakpoint_for_addr_match(1, 0x181aa6); // no d11 access in external_interrupt handler
	//dbg_set_breakpoint_for_addr_match(1, 0x27550); // d11 access in called_by_interrupt_handler_calls_wlc_dpc_caller
	//dbg_set_breakpoint_for_addr_match(2, 0x182750);
//	dbg_set_breakpoint_for_addr_match(2, 0x1926b8);
	//dbg_set_breakpoint_for_addr_match(1, 0x193744);
	//dbg_set_breakpoint_for_addr_match(1, 0x18452A);
	//dbg_set_watchpoint_for_addr_match_with_mask(0, 0x18001000, DBGWCR_MASK_00000FFF);
}

__attribute__((naked)) void
before_before_initialize_memory_hook(void)
{
	asm(//"push {r0-r3,lr}\n"					// save the registers that might change as well as the link register
		//"bl try_to_access_d11\n"
		//"pop {r0-r3,lr}\n"					// restore the saved registers
		"push {r4, lr}\n"
		"push {r0-r3,lr}\n"
		"bl set_debug_registers\n"
		"pop {r0-r3,lr}\n"
		//"push {r0-r3,lr}\n"
		//"bl try_to_access_d11\n"
		//"pop {r0-r3,lr}\n"
		// no d11 access
		"bl sub_1ed41c\n"						// call the function that was supposed to be called
		// no d11 access
		"bl sub_1810a8\n"						// call the function that was supposed to be called
		// no d11 access
		"bl sub_1ec7c8\n"						// call the function that was supposed to be called
		// no d11 access		
		"bl sub_1ed584\n"						// call the function that was supposed to be called
		// no d11 access
		"mov r4, r0\n"
		"bl sub_1ecab0\n"						// call the function that was supposed to be called
		"bl sub_1ec6fc\n"						// call the function that was supposed to be called
		"bl sub_1816e4\n"						// call the function that was supposed to be called
		"mov r0, r4\n"
		"pop {r4, lr}\n"
		"b sub_166b4\n"
		);
}

#define NEX_READ_D11_OBJMEM			15
#define OBJMEM_READ_REQUEST_GET_LEN(buf) ((*(buf) & 0xFFF00000) >> 20)
#define OBJMEM_READ_REQUEST_GET_ADDR(buf) ((*(buf) & 0x0007FFFF))
#define OBJMEM_AUTOINC_ON_READ 0x02000000
#define OBJMEM_AUTOINC_ON_WRITE 0x01000000

/*
int
nex_ioctl_handler_in_c(struct wlc_info *wlc, int cmd, void *arg, unsigned int len)
{
//	printf("%s: cmd=%d, arg=%08x, len=%d\n", __FUNCTION__, cmd, (int) arg, len);

	volatile struct d11regs *regs = wlc->regs;
	int i;
	int *buf = arg;
	int segment_len;

	switch(cmd) {
		case NEX_READ_D11_OBJMEM:
			while (len > 4) {
				segment_len = OBJMEM_READ_REQUEST_GET_LEN(buf);
				if (segment_len == 0) break;
				regs->objaddr = OBJMEM_AUTOINC_ON_READ | OBJMEM_AUTOINC_ON_WRITE | OBJMEM_READ_REQUEST_GET_ADDR(buf);
				regs->objaddr; // you always need to read back the new objaddr
				
				buf++;

				for (i = 0; i < segment_len; i++) {
					*buf++ = regs->objdata;
				}

				len -= segment_len * 4 + 4;
			}

			break;
	}

	return 0;
}

__attribute__((naked)) void
nex_ioctl_handler(void)
{
	asm(
		"mov r0, r4\n"							// Store wlc in r0
		"mov r1, r8\n"							// Store cmd in r1
		"mov r2, r5\n"							// Store arg in r2
		"mov r3, r7\n"							// Store len in r3
		"bl nex_ioctl_handler_in_c\n"
		"add sp, #364\n"
		"pop {r4-r11, pc}\n"					// return from wlc_ioctl
		);
}
*/

volatile char ucodeloader[4] = { 0 };

void
wlc_ucode_download_hook(struct wlc_hw_info *wlc_hw)
{
	volatile int *objaddr = (int *) 0x18001160;
	volatile int *objdata = (int *) 0x18001164;
	int i;

	ucodeloader[1] = 1; // tell the driver that the firmware is ready

	if(wlc_hw->ucode_loaded != 1) {
		while(1) {
			while(ucodeloader[0] != 1); // Wait for driver to be ready
			if (ucodeloader[2] != 1) break;
			wlc_ucode_write(wlc_hw, (int *) 0x1fd824, *(int *) 0x1fd820);
			*(int *) 0x180F30 = 1; // do not reset objaddr on next wlc_ucode_write call
			ucodeloader[0] = 0;
		}
		*(int *) 0x180F30 = 0; // enable resetting the objaddr on next wlc_ucode_write call
		wlc_hw->ucode_loaded = 1;
	}

	*objaddr = OBJMEM_AUTOINC_ON_READ;

	for (i = 0; i < 10; i++) {
		printf("C%08x %08x\n", *objaddr, *objdata);
	}
}


unsigned char bdc_ethernet_ipv6_udp_header_array[] = {
  0x20, 0x00, 0x00, 0x00,		/* BDC Header */
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,	/* ETHERNET: Destination MAC Address */
  'N', 'E', 'X', 'M', 'O', 'N',		/* ETHERNET: Source MAC Address */
  0x86, 0xDD,				/* ETHERNET: Type */
  0x60, 0x00, 0x00, 0x00,		/* IPv6: Version / Traffic Class / Flow Label */
  0x00, 0x08,				/* IPv6: Payload Length */
  0x88,					/* IPv6: Next Header = UDPLite */
  0x01,					/* IPv6: Hop Limit */
  0xFF, 0x02, 0x00, 0x00,		/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x00,		/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x00,		/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x01,		/* IPv6: Source IP */
  0xFF, 0x02, 0x00, 0x00,		/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x00,		/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x00,		/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x01,		/* IPv6: Destination IP */
  0xD6, 0xD8,				/* UDPLITE: Source Port = 55000 */
  0xD6, 0xD8,				/* UDPLITE: Destination Port = 55000 */
  0x00, 0x08,				/* UDPLITE: Checksum Coverage */
  0x52, 0x46,				/* UDPLITE: Checksum only over UDPLITE header*/
};

/*
struct sk_buff *
create_frame(unsigned int hooked_fct, unsigned int arg0, unsigned int arg1, unsigned int arg2, void *start_address, unsigned int length) {
	struct sk_buff *p = 0;
	struct osl_info *osh = OSL_INFO_ADDR;
	struct bdc_ethernet_ipv6_udp_header *hdr;
	struct nexmon_header *nexmon_hdr;

	p = pkt_buf_get_skb(osh, sizeof(struct bdc_ethernet_ipv6_udp_header) - 1 + sizeof(struct nexmon_header) - 1 + length);

	// copy headers to target buffer
	memcpy(p->data, bdc_ethernet_ipv6_udp_header_array, sizeof(bdc_ethernet_ipv6_udp_header_array));

	hdr = p->data;
	hdr->ipv6.payload_length = htons(sizeof(struct udp_header) + sizeof(struct nexmon_header) - 1 + length);
	nexmon_hdr = (struct nexmon_header *) hdr->payload;

	nexmon_hdr->hooked_fct = hooked_fct;
	nexmon_hdr->args[0] = arg0;
	nexmon_hdr->args[1] = arg1;
	nexmon_hdr->args[2] = arg2;

	memcpy(nexmon_hdr->payload, start_address, length);

	return p;
}
*/