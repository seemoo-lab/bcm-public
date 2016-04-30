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

static uint8_t deadbeef_at_5[16] = {
	0, 0xf1, 2, 0xf3, 4, 0xde, 0xad, 0xbe, 0xef, 0xff
};

static void
test_ld_abs(void)
{
	static struct bpf_insn insns[3][2] = {
		{
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 5),
			BPF_STMT(BPF_RET+BPF_A, 0)
		},
		{
			BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 5),
			BPF_STMT(BPF_RET+BPF_A, 0)
		},
		{
			BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 5),
			BPF_STMT(BPF_RET+BPF_A, 0)
		}
	};

	static size_t lengths[3] = { 1, 2, 4 };
	static unsigned int expected[3] = { 0xde, 0xdead, 0xdeadbeef };

	size_t i, l;
	uint8_t *pkt = deadbeef_at_5;
	size_t pktsize = sizeof(deadbeef_at_5);

	size_t insn_count = sizeof(insns[0]) / sizeof(insns[0][0]);

	for (i = 0; i < 3; i++) {
		bpfjit_func_t code;

		CHECK(bpf_validate(insns[i], insn_count));

		code = bpfjit_generate_code(NULL, insns[i], insn_count);
		REQUIRE(code != NULL);

		for (l = 0; l < 5 + lengths[i]; l++) {
			CHECK(jitcall(code, pkt, l, l) == 0);
			CHECK(jitcall(code, pkt, pktsize, l) == 0);
		}

		l = 5 + lengths[i];
		CHECK(jitcall(code, pkt, l, l) == expected[i]);
		CHECK(jitcall(code, pkt, pktsize, l) == expected[i]);

		l = pktsize;
		CHECK(jitcall(code, pkt, l, l) == expected[i]);

		bpfjit_free_code(code);
	}
}

static void
test_ld_abs_k_overflow(void)
{
	static struct bpf_insn insns[12][3] = {
		{
			BPF_STMT(BPF_LD+BPF_H+BPF_ABS, UINT32_MAX),
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_H+BPF_ABS, UINT32_MAX - 1),
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_W+BPF_ABS, UINT32_MAX),
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_W+BPF_ABS, UINT32_MAX - 1),
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_W+BPF_ABS, UINT32_MAX - 2),
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_W+BPF_ABS, UINT32_MAX - 3),
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_LD+BPF_H+BPF_ABS, UINT32_MAX),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_LD+BPF_H+BPF_ABS, UINT32_MAX - 1),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_LD+BPF_W+BPF_ABS, UINT32_MAX),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_LD+BPF_W+BPF_ABS, UINT32_MAX - 1),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_LD+BPF_W+BPF_ABS, UINT32_MAX - 2),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 7),
			BPF_STMT(BPF_LD+BPF_W+BPF_ABS, UINT32_MAX - 3),
			BPF_STMT(BPF_RET+BPF_K, 1)
		}
	};

	int i;
	uint8_t pkt[8] = { 0 };

	size_t insn_count = sizeof(insns[0]) / sizeof(insns[0][0]);

	for (i = 0; i < 3; i++) {
		bpfjit_func_t code;

		CHECK(bpf_validate(insns[i], insn_count));

		code = bpfjit_generate_code(NULL, insns[i], insn_count);
		REQUIRE(code != NULL);

		CHECK(jitcall(code, pkt, 8, 8) == 0);

		bpfjit_free_code(code);
	}
}

static void
test_ld_ind(void)
{
	static struct bpf_insn insns[6][3] = {
		{
			BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 3),
			BPF_STMT(BPF_LD+BPF_B+BPF_IND, 2),
			BPF_STMT(BPF_RET+BPF_A, 0)
		},
		{
			BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 3),
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 2),
			BPF_STMT(BPF_RET+BPF_A, 0)
		},
		{
			BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 3),
			BPF_STMT(BPF_LD+BPF_W+BPF_IND, 2),
			BPF_STMT(BPF_RET+BPF_A, 0)
		},
		{
			BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 5),
			BPF_STMT(BPF_LD+BPF_B+BPF_IND, 0),
			BPF_STMT(BPF_RET+BPF_A, 0)
		},
		{
			BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 5),
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 0),
			BPF_STMT(BPF_RET+BPF_A, 0)
		},
		{
			BPF_STMT(BPF_LDX+BPF_W+BPF_IMM, 5),
			BPF_STMT(BPF_LD+BPF_W+BPF_IND, 0),
			BPF_STMT(BPF_RET+BPF_A, 0)
		}
	};

	static size_t lengths[6] = { 1, 2, 4, 1, 2, 4 };

	static unsigned int expected[6] = {
		0xde, 0xdead, 0xdeadbeef,
		0xde, 0xdead, 0xdeadbeef
	};

	size_t i, l;
	uint8_t *pkt = deadbeef_at_5;
	size_t pktsize = sizeof(deadbeef_at_5);

	size_t insn_count = sizeof(insns[0]) / sizeof(insns[0][0]);

	for (i = 0; i < 3; i++) {
		bpfjit_func_t code;

		CHECK(bpf_validate(insns[i], insn_count));

		code = bpfjit_generate_code(NULL, insns[i], insn_count);
		REQUIRE(code != NULL);

		for (l = 0; l < 5 + lengths[i]; l++) {
			CHECK(jitcall(code, pkt, l, l) == 0);
			CHECK(jitcall(code, pkt, pktsize, l) == 0);
		}

		l = 5 + lengths[i];
		CHECK(jitcall(code, pkt, l, l) == expected[i]);
		CHECK(jitcall(code, pkt, pktsize, l) == expected[i]);

		l = pktsize;
		CHECK(jitcall(code, pkt, l, l) == expected[i]);

		bpfjit_free_code(code);
	}
}

static void
test_ld_ind_k_overflow(void)
{
	static struct bpf_insn insns[12][3] = {
		{
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, UINT32_MAX),
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, UINT32_MAX - 1),
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_W+BPF_IND, UINT32_MAX),
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_W+BPF_IND, UINT32_MAX - 1),
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_W+BPF_IND, UINT32_MAX - 2),
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_W+BPF_IND, UINT32_MAX - 3),
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, UINT32_MAX),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, UINT32_MAX - 1),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_LD+BPF_W+BPF_IND, UINT32_MAX),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_LD+BPF_W+BPF_IND, UINT32_MAX - 1),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_LD+BPF_W+BPF_IND, UINT32_MAX - 2),
			BPF_STMT(BPF_RET+BPF_K, 1)
		},
		{
			BPF_STMT(BPF_LD+BPF_H+BPF_IND, 7),
			BPF_STMT(BPF_LD+BPF_W+BPF_IND, UINT32_MAX - 3),
			BPF_STMT(BPF_RET+BPF_K, 1)
		}
	};

	int i;
	uint8_t pkt[8] = { 0 };

	size_t insn_count = sizeof(insns[0]) / sizeof(insns[0][0]);

	for (i = 0; i < 3; i++) {
		bpfjit_func_t code;

		CHECK(bpf_validate(insns[i], insn_count));

		code = bpfjit_generate_code(NULL, insns[i], insn_count);
		REQUIRE(code != NULL);

		CHECK(jitcall(code, pkt, 8, 8) == 0);

		bpfjit_free_code(code);
	}
}

static void
test_ld_len(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	size_t i;
	bpfjit_func_t code;
	uint8_t pkt[32]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 0; i < sizeof(pkt); i++)
		CHECK(jitcall(code, pkt, i, 1) == i);

	bpfjit_free_code(code);
}

static void
test_ld_imm(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_IMM, UINT32_MAX),
		BPF_STMT(BPF_RET+BPF_A, 0)
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
test_ld_ind_x_overflow1(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_LEN, 0),
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_K, UINT32_C(0xffffffff)),
		BPF_STMT(BPF_MISC+BPF_TAX, 0),
		BPF_STMT(BPF_LD+BPF_B+BPF_IND, 0),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	size_t i;
	bpfjit_func_t code;
	uint8_t pkt[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 1; i <= sizeof(pkt); i++) {
		CHECK(bpf_filter(insns, pkt, i, i) == 10 * i);
		CHECK(jitcall(code, pkt, i, i) == 10 * i);
	}

	bpfjit_free_code(code);
}

static void
test_ld_ind_x_overflow2(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_LEN, 0),
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_K, UINT32_C(0xffffffff)),
		BPF_STMT(BPF_ST, 3),
		BPF_STMT(BPF_LDX+BPF_W+BPF_MEM, 3),
		BPF_STMT(BPF_LD+BPF_B+BPF_IND, 0),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	size_t i;
	bpfjit_func_t code;
	uint8_t pkt[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 1; i <= sizeof(pkt); i++) {
		CHECK(bpf_filter(insns, pkt, i, i) == 10 * i);
		CHECK(jitcall(code, pkt, i, i) == 10 * i);
	}

	bpfjit_free_code(code);
}

void
test_ld(void)
{

	test_ld_abs();
	test_ld_abs_k_overflow();
	test_ld_ind();
	test_ld_ind_k_overflow();
	test_ld_ind_x_overflow1();
	test_ld_ind_x_overflow2();
	test_ld_len();
	test_ld_imm();
}
