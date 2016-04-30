/*-
 * Copyright (c) 2014 Alexander Nasonov.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define __BPF_PRIVATE
#include <bpfjit.h>

#include <stdbool.h>
#include <stdint.h>

#include "util.h"
#include "tests.h"

static uint32_t retM(const bpf_ctx_t *bc, bpf_args_t *args, uint32_t A);

static const bpf_copfunc_t copfuncs[] = {
	&retM
};

static const bpf_ctx_t ctx = {
	.copfuncs = copfuncs,
	.nfuncs = sizeof(copfuncs) / sizeof(copfuncs[0]),
	.extwords = 4,
	.preinited = BPF_MEMWORD_INIT(0) | BPF_MEMWORD_INIT(3),
};

static uint32_t
retM(const bpf_ctx_t *bc, bpf_args_t *args, uint32_t A)
{

	return args->mem[(uintptr_t)args->arg];
}

static void
test_cop_ret_mem(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_IMM, 13),
		BPF_STMT(BPF_ST, 2),
		BPF_STMT(BPF_LD+BPF_IMM, 137),
		BPF_STMT(BPF_ST, 1),
		BPF_STMT(BPF_MISC+BPF_COP, 0), // retM
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	bpfjit_func_t code;
	uint8_t pkt[1] = { 0 };
	uint32_t mem[ctx.extwords];
	void *arg = (void*)(uintptr_t)2;

	/* Pre-inited words. */
	mem[0] = 0;
	mem[3] = 3;

	bpf_args_t args = {
		.pkt = pkt,
		.buflen = sizeof(pkt),
		.wirelen = sizeof(pkt),
		.arg = arg,
		.mem = mem,
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	// CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(&ctx, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(code(&ctx, &args) == 13);

	bpfjit_free_code(code);
}

static void
test_cop_ret_preinited_mem(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_IMM, 13),
		BPF_STMT(BPF_ST, 2),
		BPF_STMT(BPF_LD+BPF_IMM, 137),
		BPF_STMT(BPF_ST, 1),
		BPF_STMT(BPF_MISC+BPF_COP, 0), // retM
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	bpfjit_func_t code;
	uint8_t pkt[1] = { 0 };
	uint32_t mem[ctx.extwords];
	void *arg = (void*)(uintptr_t)3;

	/* Pre-inited words. */
	mem[0] = 0;
	mem[3] = 3;

	bpf_args_t args = {
		.pkt = pkt,
		.buflen = sizeof(pkt),
		.wirelen = sizeof(pkt),
		.arg = arg,
		.mem = mem,
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	// CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(&ctx, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(code(&ctx, &args) == 3);

	bpfjit_free_code(code);
}

void
test_cop_extmem(void)
{

	test_cop_ret_mem();
	test_cop_ret_preinited_mem();
}
