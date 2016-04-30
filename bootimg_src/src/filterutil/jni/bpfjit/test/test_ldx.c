/*-
 * Copyright (c) 2011-2012 Alexander Nasonov.
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

#include <bpfjit.h>

#include <stdint.h>

#include "util.h"
#include "tests.h"

static void
test_ldx_imm1(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, UINT32_MAX - 5),
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_X, 0),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	bpfjit_func_t code;
	uint8_t pkt[1]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == UINT32_MAX - 5);

	bpfjit_free_code(code);
}

static void
test_ldx_imm2(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 5),
		BPF_STMT(BPF_LD+BPF_IMM, 5),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 1, 0),
		BPF_STMT(BPF_RET+BPF_K, 7),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX)
	};

	bpfjit_func_t code;
	uint8_t pkt[1]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == UINT32_MAX);

	bpfjit_free_code(code);
}

static void
test_ldx_len1(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LDX+BPF_W+BPF_LEN, 0),
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_X, 0),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	size_t i;
	bpfjit_func_t code;
	uint8_t pkt[5]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 1; i < sizeof(pkt); i++) {
		CHECK(jitcall(code, pkt, i, 1) == i);
		CHECK(jitcall(code, pkt, i + 1, i) == i + 1);
	}

	bpfjit_free_code(code);
}

static void
test_ldx_len2(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LDX+BPF_W+BPF_LEN, 0),
		BPF_STMT(BPF_LD+BPF_IMM, 5),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 1, 0),
		BPF_STMT(BPF_RET+BPF_K, 7),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX)
	};

	bpfjit_func_t code;
	uint8_t pkt[5]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 5, 1) == UINT32_MAX);
	CHECK(jitcall(code, pkt, 6, 5) == 7);

	bpfjit_free_code(code);
}

static void
test_ldx_msh(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LDX+BPF_B+BPF_MSH, 1),
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_X, 0),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	bpfjit_func_t code;
	uint8_t pkt[2] = { 0, 0x7a };

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 2, 2) == 40);

	bpfjit_free_code(code);
}

void
test_ldx(void)
{

	test_ldx_imm1();
	test_ldx_imm2();
	test_ldx_len1();
	test_ldx_len2();
	test_ldx_msh();
}
