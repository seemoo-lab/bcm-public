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
test_jmp_ja(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_JMP+BPF_JA, 1),
		BPF_STMT(BPF_RET+BPF_K, 0),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX),
		BPF_STMT(BPF_RET+BPF_K, 1),
		BPF_STMT(BPF_RET+BPF_K, 2),
		BPF_STMT(BPF_RET+BPF_K, 3),
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
test_jmp_gt_k(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, 7, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 0),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, 2, 2, 0),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, 9, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 1),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, 4, 1, 1),
		BPF_STMT(BPF_RET+BPF_K, 2),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, 6, 2, 3),
		BPF_STMT(BPF_RET+BPF_K, 3),
		BPF_STMT(BPF_RET+BPF_K, 4),
		BPF_STMT(BPF_RET+BPF_K, 5),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, 5, 3, 1),
		BPF_STMT(BPF_RET+BPF_K, 6),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, 0, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 7),
		BPF_STMT(BPF_RET+BPF_K, 8)
	};

	bpfjit_func_t code;
	uint8_t pkt[8]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == 1);
	CHECK(jitcall(code, pkt, 2, 2) == 1);
	CHECK(jitcall(code, pkt, 3, 3) == 7);
	CHECK(jitcall(code, pkt, 4, 4) == 7);
	CHECK(jitcall(code, pkt, 5, 5) == 7);
	CHECK(jitcall(code, pkt, 6, 6) == 8);
	CHECK(jitcall(code, pkt, 7, 7) == 5);
	CHECK(jitcall(code, pkt, 8, 8) == 0);

	bpfjit_free_code(code);
}

static void
test_jmp_ge_k(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, 8, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 0),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, 3, 2, 0),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, 9, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 1),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, 5, 1, 1),
		BPF_STMT(BPF_RET+BPF_K, 2),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, 7, 2, 3),
		BPF_STMT(BPF_RET+BPF_K, 3),
		BPF_STMT(BPF_RET+BPF_K, 4),
		BPF_STMT(BPF_RET+BPF_K, 5),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, 6, 3, 1),
		BPF_STMT(BPF_RET+BPF_K, 6),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, 1, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 7),
		BPF_STMT(BPF_RET+BPF_K, 8)
	};

	bpfjit_func_t code;
	uint8_t pkt[8]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == 1);
	CHECK(jitcall(code, pkt, 2, 2) == 1);
	CHECK(jitcall(code, pkt, 3, 3) == 7);
	CHECK(jitcall(code, pkt, 4, 4) == 7);
	CHECK(jitcall(code, pkt, 5, 5) == 7);
	CHECK(jitcall(code, pkt, 6, 6) == 8);
	CHECK(jitcall(code, pkt, 7, 7) == 5);
	CHECK(jitcall(code, pkt, 8, 8) == 0);

	bpfjit_free_code(code);
}

static void
test_jmp_eq_k(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 8, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 0),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 3, 1, 0),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 9, 1, 1),
		BPF_STMT(BPF_RET+BPF_K, 1),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 5, 1, 1),
		BPF_STMT(BPF_RET+BPF_K, 2),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 7, 2, 3),
		BPF_STMT(BPF_RET+BPF_K, 3),
		BPF_STMT(BPF_RET+BPF_K, 4),
		BPF_STMT(BPF_RET+BPF_K, 5),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 6, 3, 1),
		BPF_STMT(BPF_RET+BPF_K, 6),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 1, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 7),
		BPF_STMT(BPF_RET+BPF_K, 8)
	};

	bpfjit_func_t code;
	uint8_t pkt[8]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == 7);
	CHECK(jitcall(code, pkt, 2, 2) == 7);
	CHECK(jitcall(code, pkt, 3, 3) == 1);
	CHECK(jitcall(code, pkt, 4, 4) == 7);
	CHECK(jitcall(code, pkt, 5, 5) == 7);
	CHECK(jitcall(code, pkt, 6, 6) == 8);
	CHECK(jitcall(code, pkt, 7, 7) == 5);
	CHECK(jitcall(code, pkt, 8, 8) == 0);

	bpfjit_free_code(code);
}

static void
test_jmp_modulo_k(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_IMM, UINT32_C(0x7fffff77)),
		BPF_STMT(BPF_ALU+BPF_LSH+BPF_K, 4),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, UINT32_C(0xfffff770), 1, 0),
		BPF_STMT(BPF_RET+BPF_K, 0),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, UINT32_C(0xfffff770), 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 1),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, UINT32_C(0xfffff771), 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 2),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, UINT32_C(0xfffff770), 0, 3),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, UINT32_C(0xfffff770), 2, 0),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, UINT32_C(0xfffff771), 1, 0),
		BPF_STMT(BPF_JMP+BPF_JA, 1),
		BPF_STMT(BPF_RET+BPF_K, 3),

		/* FFFFF770+FFFFF770 = 00000001,FFFFEEE0 */
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_K, UINT32_C(0xfffff770)),

		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, UINT32_C(0xffffeee0), 1, 0),
		BPF_STMT(BPF_RET+BPF_K, 4),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, UINT32_C(0xffffeee0), 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 5),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, UINT32_C(0xffffeee1), 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 6),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, UINT32_C(0xffffeee0), 0, 3),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, UINT32_C(0xffffeee0), 2, 0),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_K, UINT32_C(0xffffeee1), 1, 0),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX),
		BPF_STMT(BPF_RET+BPF_K, 7)
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
test_jmp_jset_k(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 8, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 0),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 4, 2, 0),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 3, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 1),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 2, 1, 1),
		BPF_STMT(BPF_RET+BPF_K, 2),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 1, 2, 3),
		BPF_STMT(BPF_RET+BPF_K, 3),
		BPF_STMT(BPF_RET+BPF_K, 4),
		BPF_STMT(BPF_RET+BPF_K, 5),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 2, 3, 1),
		BPF_STMT(BPF_RET+BPF_K, 6),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 7, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 7),
		BPF_STMT(BPF_RET+BPF_K, 8)
	};

	bpfjit_func_t code;
	uint8_t pkt[8]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == 1);
	CHECK(jitcall(code, pkt, 2, 2) == 1);
	CHECK(jitcall(code, pkt, 3, 3) == 1);
	CHECK(jitcall(code, pkt, 4, 4) == 7);
	CHECK(jitcall(code, pkt, 5, 5) == 5);
	CHECK(jitcall(code, pkt, 6, 6) == 8);
	CHECK(jitcall(code, pkt, 7, 7) == 5);
	CHECK(jitcall(code, pkt, 8, 8) == 0);

	bpfjit_free_code(code);
}

static void
test_jmp_gt_x(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 7),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_X, 0, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 2),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_X, 0, 3, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 9),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_X, 0, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 1),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 4),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_X, 0, 1, 1),
		BPF_STMT(BPF_RET+BPF_K, 2),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 6),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_X, 0, 2, 3),
		BPF_STMT(BPF_RET+BPF_K, 3),
		BPF_STMT(BPF_RET+BPF_K, 4),
		BPF_STMT(BPF_RET+BPF_K, 5),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 5),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_X, 0, 4, 1),
		BPF_STMT(BPF_RET+BPF_K, 6),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 0),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_X, 0, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 7),
		BPF_STMT(BPF_RET+BPF_K, 8)
	};

	bpfjit_func_t code;
	uint8_t pkt[8]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == 1);
	CHECK(jitcall(code, pkt, 2, 2) == 1);
	CHECK(jitcall(code, pkt, 3, 3) == 7);
	CHECK(jitcall(code, pkt, 4, 4) == 7);
	CHECK(jitcall(code, pkt, 5, 5) == 7);
	CHECK(jitcall(code, pkt, 6, 6) == 8);
	CHECK(jitcall(code, pkt, 7, 7) == 5);
	CHECK(jitcall(code, pkt, 8, 8) == 0);

	bpfjit_free_code(code);
}

static void
test_jmp_ge_x(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 8),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_X, 0, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 3),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_X, 0, 3, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 9),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_X, 0, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 1),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 5),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_X, 0, 1, 1),
		BPF_STMT(BPF_RET+BPF_K, 2),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 7),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_X, 0, 2, 3),
		BPF_STMT(BPF_RET+BPF_K, 3),
		BPF_STMT(BPF_RET+BPF_K, 4),
		BPF_STMT(BPF_RET+BPF_K, 5),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 6),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_X, 0, 4, 1),
		BPF_STMT(BPF_RET+BPF_K, 6),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 1),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_X, 0, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 7),
		BPF_STMT(BPF_RET+BPF_K, 8)
	};

	bpfjit_func_t code;
	uint8_t pkt[8]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == 1);
	CHECK(jitcall(code, pkt, 2, 2) == 1);
	CHECK(jitcall(code, pkt, 3, 3) == 7);
	CHECK(jitcall(code, pkt, 4, 4) == 7);
	CHECK(jitcall(code, pkt, 5, 5) == 7);
	CHECK(jitcall(code, pkt, 6, 6) == 8);
	CHECK(jitcall(code, pkt, 7, 7) == 5);
	CHECK(jitcall(code, pkt, 8, 8) == 0);

	bpfjit_free_code(code);
}

static void
test_jmp_eq_x(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 8),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 3),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 2, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 9),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 1, 1),
		BPF_STMT(BPF_RET+BPF_K, 1),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 5),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 1, 1),
		BPF_STMT(BPF_RET+BPF_K, 2),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 7),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 2, 3),
		BPF_STMT(BPF_RET+BPF_K, 3),
		BPF_STMT(BPF_RET+BPF_K, 4),
		BPF_STMT(BPF_RET+BPF_K, 5),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 6),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 3, 1),
		BPF_STMT(BPF_RET+BPF_K, 6),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 1, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 7),
		BPF_STMT(BPF_RET+BPF_K, 8)
	};

	bpfjit_func_t code;
	uint8_t pkt[8]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == 7);
	CHECK(jitcall(code, pkt, 2, 2) == 7);
	CHECK(jitcall(code, pkt, 3, 3) == 1);
	CHECK(jitcall(code, pkt, 4, 4) == 7);
	CHECK(jitcall(code, pkt, 5, 5) == 7);
	CHECK(jitcall(code, pkt, 6, 6) == 8);
	CHECK(jitcall(code, pkt, 7, 7) == 5);
	CHECK(jitcall(code, pkt, 8, 8) == 0);

	bpfjit_free_code(code);
}

static void
test_jmp_jset_x(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 8),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_X, 0, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 4),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_X, 0, 2, 0),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_X, 3, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 1),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 2),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_X, 0, 1, 1),
		BPF_STMT(BPF_RET+BPF_K, 2),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 1),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_X, 0, 2, 3),
		BPF_STMT(BPF_RET+BPF_K, 3),
		BPF_STMT(BPF_RET+BPF_K, 4),
		BPF_STMT(BPF_RET+BPF_K, 5),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 2),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_X, 0, 4, 1),
		BPF_STMT(BPF_RET+BPF_K, 6),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 7),
		BPF_JUMP(BPF_JMP+BPF_JSET+BPF_X, 0, 0, 0),
		BPF_STMT(BPF_RET+BPF_K, 7),
		BPF_STMT(BPF_RET+BPF_K, 8)
	};

	bpfjit_func_t code;
	uint8_t pkt[8]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == 1);
	CHECK(jitcall(code, pkt, 2, 2) == 1);
	CHECK(jitcall(code, pkt, 3, 3) == 1);
	CHECK(jitcall(code, pkt, 4, 4) == 7);
	CHECK(jitcall(code, pkt, 5, 5) == 5);
	CHECK(jitcall(code, pkt, 6, 6) == 8);
	CHECK(jitcall(code, pkt, 7, 7) == 5);
	CHECK(jitcall(code, pkt, 8, 8) == 0);

	bpfjit_free_code(code);
}

static void
test_jmp_modulo_x(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_IMM, UINT32_C(0x7fffff77)),
		/* FFFFF770 << 4 = FFFFF770 */
		BPF_STMT(BPF_ALU+BPF_LSH+BPF_K, 4),

		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, UINT32_C(0xfffff770)),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 1, 0),
		BPF_STMT(BPF_RET+BPF_K, 0),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_X, 0, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 1),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, UINT32_C(0xfffff771)),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_X, 0, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 2),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, UINT32_C(0xfffff770)),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 0, 4),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_X, 0, 3, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, UINT32_C(0xfffff771)),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_X, 0, 1, 0),
		BPF_STMT(BPF_JMP+BPF_JA, 1),
		BPF_STMT(BPF_RET+BPF_K, 3),

		/* FFFFF770+FFFFF770 = 00000001,FFFFEEE0 */
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_K, UINT32_C(0xfffff770)),

		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, UINT32_C(0xffffeee0)),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 1, 0),
		BPF_STMT(BPF_RET+BPF_K, 4),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_X, 0, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 5),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, UINT32_C(0xffffeee1)),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_X, 0, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, 6),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, UINT32_C(0xffffeee0)),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_X, 0, 0, 4),
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_X, 0, 3, 0),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, UINT32_C(0xffffeee1)),
		BPF_JUMP(BPF_JMP+BPF_JGE+BPF_X, 0, 1, 0),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX),
		BPF_STMT(BPF_RET+BPF_K, 7)
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

void
test_jmp(void)
{

	test_jmp_ja();
	test_jmp_gt_k();
	test_jmp_ge_k();
	test_jmp_eq_k();
	test_jmp_jset_k();
	test_jmp_modulo_k();
	test_jmp_gt_x();
	test_jmp_ge_x();
	test_jmp_eq_x();
	test_jmp_jset_x();
	test_jmp_modulo_x();
}
