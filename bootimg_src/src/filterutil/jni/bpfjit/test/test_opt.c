/*-
 * Copyright (c) 2012 Alexander Nasonov.
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

/*
 * Test length check optimization.
 */

#include <bpfjit.h>

#include <stdint.h>

#include "util.h"
#include "tests.h"

static void
test_opt_ld_abs_1(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x800, 0, 8),
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 26),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 2),
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 30),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 3, 4),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 0, 3),
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 30),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX),
		BPF_STMT(BPF_RET+BPF_K, 0),
	};

	size_t i, j;
	bpfjit_func_t code;
	uint8_t pkt[2][34] = {
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x0f,
			0x80, 0x03, 0x70, 0x23
		},
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x23,
			0x80, 0x03, 0x70, 0x0f
		}
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 0; i < 2; i++) {
		for (j = 1; j < sizeof(pkt[i]); j++)
			CHECK(jitcall(code, pkt[i], j, j) == 0);
		CHECK(jitcall(code, pkt[i], j, j) == UINT32_MAX);
	}

	bpfjit_free_code(code);
}

static void
test_opt_ld_abs_2(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 26),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 2),
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 30),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 3, 6),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 0, 5),
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 30),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 3),
		BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x800, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX),
		BPF_STMT(BPF_RET+BPF_K, 0),
	};

	size_t i, j;
	bpfjit_func_t code;
	uint8_t pkt[2][34] = {
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x0f,
			0x80, 0x03, 0x70, 0x23
		},
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x23,
			0x80, 0x03, 0x70, 0x0f
		}
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 0; i < 2; i++) {
		for (j = 1; j < sizeof(pkt[i]); j++)
			CHECK(jitcall(code, pkt[i], j, j) == 0);
		CHECK(jitcall(code, pkt[i], j, j) == UINT32_MAX);
	}

	bpfjit_free_code(code);
}

static void
test_opt_ld_abs_3(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 30),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 0, 2),
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 26),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 3, 6),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 5),
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 26),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 0, 3),
		BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x800, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX),
		BPF_STMT(BPF_RET+BPF_K, 0),
	};

	size_t i, j;
	bpfjit_func_t code;
	uint8_t pkt[2][34] = {
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x0f,
			0x80, 0x03, 0x70, 0x23
		},
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x23,
			0x80, 0x03, 0x70, 0x0f
		}
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 0; i < 2; i++) {
		for (j = 1; j < sizeof(pkt[i]); j++)
			CHECK(jitcall(code, pkt[i], j, j) == 0);
		CHECK(jitcall(code, pkt[i], j, j) == UINT32_MAX);
	}

	bpfjit_free_code(code);
}

static void
test_opt_ld_ind_1(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 12),
		BPF_STMT(BPF_LD+BPF_H+BPF_IND, 0),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x800, 0, 8),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 14),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 2),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 18),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 3, 4),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 0, 3),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 18),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX),
		BPF_STMT(BPF_RET+BPF_K, 0),
	};

	size_t i, j;
	bpfjit_func_t code;
	uint8_t pkt[2][34] = {
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x0f,
			0x80, 0x03, 0x70, 0x23
		},
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x23,
			0x80, 0x03, 0x70, 0x0f
		}
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 0; i < 2; i++) {
		for (j = 1; j < sizeof(pkt[i]); j++)
			CHECK(jitcall(code, pkt[i], j, j) == 0);
		CHECK(jitcall(code, pkt[i], j, j) == UINT32_MAX);
	}

	bpfjit_free_code(code);
}

static void
test_opt_ld_ind_2(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 0),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 26),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 2),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 30),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 3, 6),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 0, 5),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 30),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 3),
		BPF_STMT(BPF_LD+BPF_H+BPF_IND, 12),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x800, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX),
		BPF_STMT(BPF_RET+BPF_K, 0),
	};

	size_t i, j;
	bpfjit_func_t code;
	uint8_t pkt[2][34] = {
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x0f,
			0x80, 0x03, 0x70, 0x23
		},
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x23,
			0x80, 0x03, 0x70, 0x0f
		}
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 0; i < 2; i++) {
		for (j = 1; j < sizeof(pkt[i]); j++)
			CHECK(jitcall(code, pkt[i], j, j) == 0);
		CHECK(jitcall(code, pkt[i], j, j) == UINT32_MAX);
	}

	bpfjit_free_code(code);
}

static void
test_opt_ld_ind_3(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 15),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 15),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 0, 2),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 11),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 3, 7),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 6),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 11),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 0, 4),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 0),
		BPF_STMT(BPF_LD+BPF_H+BPF_IND, 12),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x800, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX),
		BPF_STMT(BPF_RET+BPF_K, 0),
	};

	size_t i, j;
	bpfjit_func_t code;
	uint8_t pkt[2][34] = {
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x0f,
			0x80, 0x03, 0x70, 0x23
		},
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x23,
			0x80, 0x03, 0x70, 0x0f
		}
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 0; i < 2; i++) {
		for (j = 1; j < sizeof(pkt[i]); j++)
			CHECK(jitcall(code, pkt[i], j, j) == 0);
		CHECK(jitcall(code, pkt[i], j, j) == UINT32_MAX);
	}

	bpfjit_free_code(code);
}

static void
test_opt_ld_ind_4(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 11),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 19),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 0, 2),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 15),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 3, 7),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 6),
		BPF_STMT(BPF_LD+BPF_W+BPF_IND, 15),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 0, 4),
		BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 0),
		BPF_STMT(BPF_LD+BPF_H+BPF_IND, 12),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x800, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, UINT32_MAX),
		BPF_STMT(BPF_RET+BPF_K, 0),
	};

	size_t i, j;
	bpfjit_func_t code;
	uint8_t pkt[2][34] = {
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x0f,
			0x80, 0x03, 0x70, 0x23
		},
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
			14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			0x80, 0x03, 0x70, 0x23,
			0x80, 0x03, 0x70, 0x0f
		}
	};

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 0; i < 2; i++) {
		for (j = 1; j < sizeof(pkt[i]); j++)
			CHECK(jitcall(code, pkt[i], j, j) == 0);
		CHECK(jitcall(code, pkt[i], j, j) == UINT32_MAX);
	}

	bpfjit_free_code(code);
}

void
test_opt(void)
{

	test_opt_ld_abs_1();
	test_opt_ld_abs_2();
	test_opt_ld_abs_3();
	test_opt_ld_ind_1();
	test_opt_ld_ind_2();
	test_opt_ld_ind_3();
	test_opt_ld_ind_4();
	/* test BPF_MSH */
}
