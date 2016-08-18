#include <bcm4339.h>
#include <debug.h>
#include <wrapper.h>
#include <structs.h>
#include <helper.h>
#include <types.h> /* needs to be included before bcmsdpcm.h */
#include <bcmdhd/bcmsdpcm.h>
#include <bcmdhd/bcmcdc.h>

unsigned char bdc_ethernet_ipv6_udp_header_array[] = {
  0x20, 0x00, 0x00, 0x00,				/* BDC Header */
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,	/* ETHERNET: Destination MAC Address */
  'N', 'E', 'X', 'M', 'O', 'N',			/* ETHERNET: Source MAC Address */
  0x86, 0xDD,							/* ETHERNET: Type */
  0x60, 0x00, 0x00, 0x00,				/* IPv6: Version / Traffic Class / Flow Label */
  0x00, 0x08,							/* IPv6: Payload Length */
  0x88,									/* IPv6: Next Header = UDPLite */
  0x01,									/* IPv6: Hop Limit */
  0xFF, 0x02, 0x00, 0x00,				/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x00,				/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x00,				/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x01,				/* IPv6: Source IP */
  0xFF, 0x02, 0x00, 0x00,				/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x00,				/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x00,				/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x01,				/* IPv6: Destination IP */
  0xD6, 0xD8,							/* UDPLITE: Source Port = 55000 */
  0xD6, 0xD8,							/* UDPLITE: Destination Port = 55000 */
  0x00, 0x08,							/* UDPLITE: Checksum Coverage */
  0x52, 0x46,							/* UDPLITE: Checksum only over UDPLITE header*/
};

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
unsigned char breakpoint_hit_limit[DBG_NUMBER_OF_BREAKPOINTS] = { 100, 1, 1, 1 };

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

void
handle_pref_abort_exception(struct trace *trace)
{
	struct sk_buff *p;

	fix_sp_lr(trace);
	
	if(dbg_is_breakpoint_enabled(0)) {
		if (dbg_triggers_on_breakpoint_address(0, trace->pc)) {
			// to continue executed on the instruction where breakpoint 0 triggered, we set the breakpoint type to address mismatch to trigger on any instruction except the breakpoint address
			dbg_set_breakpoint_type_to_instr_addr_mismatch(0);

			// Create an SDIO frame containing the stack trace encapsulated in a UDPLite6 
			// frame and send it to Android, if the chosen DMA info struct (trace->r0) 
			// address is different from the SDIO's DMA info struct. Hence, only stack
			// traces for communicating with the D11 core are generated.
			if (trace->r0 != 0x1eab98) {
				p = create_frame(trace->pc, trace->r0, trace->r1, trace->r2, (void *) trace->sp, BOTTOM_OF_STACK - (unsigned int) trace->sp);
				dngl_sendpkt(SDIO_INFO_ADDR, p, SDPCM_DATA_CHANNEL);
			}

			// to know which breakpoint mismatch was triggerd on a next breakpoint hit, we set a bit in the breakpoint_hit variable
			breakpoint_hit |= DBGBP0;
			breakpoint_hit_counter[0]++;
		} else if (breakpoint_hit & DBGBP0) {
			// we reset the the breakpoint for address matching
			dbg_set_breakpoint_type_to_instr_addr_match(0);
			if (0 && breakpoint_hit_counter[0] >= breakpoint_hit_limit[0]) {
				dbg_disable_breakpoint(0);
			}
			// we set the bit in the breakpoint_hit variable to 0
			breakpoint_hit &= ~DBGBP0;
		}
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
	dbg_set_breakpoint_for_addr_match(0, 0x1844B2); // dma_txfast
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