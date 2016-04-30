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

#include <bpfjit.h>

#include <stdint.h>
#include <string.h>

#include "util.h"
#include "tests.h"

static void
test_st1(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_STMT(BPF_ST, 0),
		BPF_STMT(BPF_LD+BPF_MEM, 0),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	size_t i;
	bpfjit_func_t code;
	uint8_t pkt[16]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (i = 1; i <= sizeof(pkt); i++)
		CHECK(jitcall(code, pkt, i, sizeof(pkt)) == i);

	bpfjit_free_code(code);
}

static void
test_st2(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_STMT(BPF_ST, BPF_MEMWORDS-1),
		BPF_STMT(BPF_LD+BPF_MEM, 0),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	bpfjit_func_t code;
	uint8_t pkt[1]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == 0);

	bpfjit_free_code(code);
}

static void
test_st3(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_STMT(BPF_ST, 0),
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_K, 100),
		BPF_STMT(BPF_ST, BPF_MEMWORDS-1),
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_K, 200),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 301, 2, 0),
		BPF_STMT(BPF_LD+BPF_MEM, BPF_MEMWORDS-1),
		BPF_STMT(BPF_RET+BPF_A, 0),
		BPF_STMT(BPF_LD+BPF_MEM, 0),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	bpfjit_func_t code;
	uint8_t pkt[2]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	REQUIRE(BPF_MEMWORDS > 1);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == 1);
	CHECK(jitcall(code, pkt, 2, 2) == 102);

	bpfjit_free_code(code);
}

static void
test_st4(void)
{
	static struct bpf_insn insns[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_LEN, 0),
		BPF_STMT(BPF_ST, 5),
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_K, 100),
		BPF_STMT(BPF_ST, BPF_MEMWORDS-1),
		BPF_STMT(BPF_ALU+BPF_ADD+BPF_K, 200),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 301, 2, 0),
		BPF_STMT(BPF_LD+BPF_MEM, BPF_MEMWORDS-1),
		BPF_STMT(BPF_RET+BPF_A, 0),
		BPF_STMT(BPF_LD+BPF_MEM, 5),
		BPF_STMT(BPF_RET+BPF_A, 0)
	};

	bpfjit_func_t code;
	uint8_t pkt[2]; /* the program doesn't read any data */

	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	REQUIRE(BPF_MEMWORDS > 6);

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	CHECK(jitcall(code, pkt, 1, 1) == 1);
	CHECK(jitcall(code, pkt, 2, 2) == 102);

	bpfjit_free_code(code);
}


static void
test_st5(void)
{
	struct bpf_insn insns[5*BPF_MEMWORDS+2];
	size_t insn_count = sizeof(insns) / sizeof(insns[0]);

	size_t k;
	bpfjit_func_t code;
	uint8_t pkt[BPF_MEMWORDS]; /* the program doesn't read any data */

	memset(insns, 0, sizeof(insns));

	/* for each k do M[k] = k */
	for (k = 0; k < BPF_MEMWORDS; k++) {
		insns[2*k].code   = BPF_LD+BPF_IMM;
		insns[2*k].k      = 3*k;
		insns[2*k+1].code = BPF_ST;
		insns[2*k+1].k    = k;
	}

	/* load wirelen into A */
	insns[2*BPF_MEMWORDS].code = BPF_LD+BPF_W+BPF_LEN;

	/* for each k, if (A == k + 1) return M[k] */
	for (k = 0; k < BPF_MEMWORDS; k++) {
		insns[2*BPF_MEMWORDS+3*k+1].code = BPF_JMP+BPF_JEQ+BPF_K;
		insns[2*BPF_MEMWORDS+3*k+1].k    = k+1;
		insns[2*BPF_MEMWORDS+3*k+1].jt   = 0;
		insns[2*BPF_MEMWORDS+3*k+1].jf   = 2;
		insns[2*BPF_MEMWORDS+3*k+2].code = BPF_LD+BPF_MEM;
		insns[2*BPF_MEMWORDS+3*k+2].k    = k;
		insns[2*BPF_MEMWORDS+3*k+3].code = BPF_RET+BPF_A;
		insns[2*BPF_MEMWORDS+3*k+3].k    = 0;
	}

	insns[5*BPF_MEMWORDS+1].code = BPF_RET+BPF_K;
	insns[5*BPF_MEMWORDS+1].k    = UINT32_MAX;

	CHECK(bpf_validate(insns, insn_count));

	code = bpfjit_generate_code(NULL, insns, insn_count);
	REQUIRE(code != NULL);

	for (k = 1; k <= sizeof(pkt); k++)
		CHECK(jitcall(code, pkt, k, k) == 3*(k-1));

	bpfjit_free_code(code);
}

void
test_st(void)
{

	test_st1();
	test_st2();
	test_st3();
	test_st4();
	test_st5();
}
