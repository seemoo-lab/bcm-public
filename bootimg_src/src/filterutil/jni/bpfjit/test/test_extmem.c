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

static const bpf_ctx_t ctx = {
	.copfuncs = NULL,
	.nfuncs = 0,
	.extwords = 4,
	.preinited = BPF_MEMWORD_INIT(0) | BPF_MEMWORD_INIT(3),
};

static void
test_extmem_load_default(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_MEM, 1),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	bpfjit_func_t code;
	uint8_t pkt[1] = { 0 };
	uint32_t mem[ctx.extwords];

	/* Pre-inited words. */
	mem[0] = 0;
	mem[3] = 3;

	bpf_args_t args = {
		.pkt = pkt,
		.buflen = sizeof(pkt),
		.wirelen = sizeof(pkt),
		.mem = mem,
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(&ctx, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(code(&ctx, &args) == 0);

	bpfjit_free_code(code);
}

static void
test_extmem_load_preinited(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_MEM, 3),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	bpfjit_func_t code;
	uint8_t pkt[1] = { 0 };
	uint32_t mem[ctx.extwords];

	/* Pre-inited words. */
	mem[0] = 0;
	mem[3] = 3;

	bpf_args_t args = {
		.pkt = pkt,
		.buflen = sizeof(pkt),
		.wirelen = sizeof(pkt),
		.mem = mem,
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	// CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(&ctx, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(code(&ctx, &args) == 3);

	bpfjit_free_code(code);
}

static void
test_extmem_invalid_load(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_MEM, 4),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpfjit_generate_code(&ctx, insns, insn_count) == NULL);
}

static void
test_extmem_store(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_IMM, 1),        /* A <- 1     */
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 2), /* X <- 2     */
		BPF_STMT(BPF_ST, 1),                /* M[1] <- A  */
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_X, 0), /* A <- A + X */
		BPF_STMT(BPF_STX, 2),               /* M[2] <- X  */
		BPF_STMT(BPF_ST, 3),                /* M[3] <- A  */
		BPF_STMT(BPF_RET+BPF_A, 0)          /* ret A      */
	};

	bpfjit_func_t code;
	uint8_t pkt[1] = { 0 };
	uint32_t mem[ctx.extwords];

	/* Pre-inited words. */
	mem[0] = 0;
	mem[3] = 7;

	mem[1] = mem[2] = 0xdeadbeef;

	bpf_args_t args = {
		.pkt = pkt,
		.buflen = sizeof(pkt),
		.wirelen = sizeof(pkt),
		.mem = mem,
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	// CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(&ctx, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(code(&ctx, &args) == 3);

	bpfjit_free_code(code);

	CHECK(mem[0] == 0);
	CHECK(mem[1] == 1);
	CHECK(mem[2] == 2);
	CHECK(mem[3] == 3);
}

static void
test_extmem_side_effect(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 0),  /* A <- P[0]  */
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 2), /* X <- 2     */
		BPF_STMT(BPF_ST, 1),                /* M[1] <- A  */
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_X, 0), /* A <- A + X */
		BPF_STMT(BPF_STX, 2),               /* M[2] <- X  */
		BPF_STMT(BPF_ST, 3),                /* M[3] <- A  */
		BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 99), /* A <- P[99] */
		BPF_STMT(BPF_RET+BPF_A, 0)          /* ret A      */
	};

	bpfjit_func_t code;
	uint8_t pkt[1] = { 1 };
	uint32_t mem[ctx.extwords];

	/* Pre-inited words. */
	mem[0] = 0;
	mem[3] = 7;

	mem[1] = mem[2] = 0xdeadbeef;

	bpf_args_t args = {
		.pkt = pkt,
		.buflen = sizeof(pkt),
		.wirelen = sizeof(pkt),
		.mem = mem,
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	// CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(&ctx, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(code(&ctx, &args) == 0);

	bpfjit_free_code(code);

	CHECK(mem[0] == 0);
	CHECK(mem[1] == 1);
	CHECK(mem[2] == 2);
	CHECK(mem[3] == 3);
}

static void
test_extmem_invalid_store(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_ST, 4),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpfjit_generate_code(&ctx, insns, insn_count) == NULL);
}

void
test_extmem(void)
{

	test_extmem_load_default();
	test_extmem_load_preinited();
	test_extmem_invalid_load();
	test_extmem_invalid_store();
	test_extmem_side_effect();
	test_extmem_store();
}
