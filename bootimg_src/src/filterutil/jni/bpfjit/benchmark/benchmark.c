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

#define __BPF_PRIVATE
#include <bpfjit.h>

#include <err.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


unsigned int filter_pkt(const bpf_ctx_t *, bpf_args_t *);

void usage(const char *prog);
void test_bpf_filter(size_t counter, size_t dummy);

void
test_fun(bpfjit_func_t fun, const uint8_t *pkt,
    unsigned int pktsize, size_t counter, size_t dummy, const char*msg);

static uint32_t
dummy_copfunc(const bpf_ctx_t *bc, bpf_args_t *args, uint32_t A)
{

	return 0;
}

static const bpf_copfunc_t copfuncs[] = {
	&dummy_copfunc
};

static bpf_ctx_t ctx = { copfuncs, sizeof(copfuncs) / sizeof(copfuncs[0]) };

/*
 * From bpf(4): This filter accepts only IP packets between host
 * 128.3.112.15 and 128.3.112.35.
 */
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
	BPF_STMT(BPF_RET+BPF_K, 0)
};

static uint8_t test_pkt[128] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0x08, 0x00,
	14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	0x80, 0x03, 0x70, 0x0f,
	0x80, 0x03, 0x70, 0x23
};

static inline
unsigned int jitcall(bpfjit_func_t fn,
    const uint8_t *pkt, unsigned int wirelen, unsigned int buflen)
{
	bpf_args_t args;

	args.pkt = pkt;
	args.wirelen = wirelen;
	args.buflen = buflen;

	return fn(NULL, &args);
}

void
test_fun(bpfjit_func_t fun, const uint8_t *pkt,
    unsigned int pktsize, size_t counter, size_t dummy, const char*msg)
{
	size_t i;
	unsigned int ret = 0;

	for (i = 0; i < counter; i++)
		ret += jitcall(fun, pkt, pktsize, pktsize);

	if (counter == dummy)
		printf("%s returned %u\n", msg, ret);
}

static void
test_c(size_t counter, const uint8_t *pkt,
    unsigned int pktsize, size_t dummy)
{

	test_fun(&filter_pkt, pkt, pktsize, counter, dummy, "C code");
}

static void
test_bpfjit(size_t counter, const uint8_t *pkt,
    unsigned int pktsize, size_t dummy)
{
	bpfjit_func_t code;

	code = bpfjit_generate_code(&ctx, insns,
	    sizeof(insns) / sizeof(insns[0]));
	test_fun(code, pkt, pktsize, counter, dummy, "bpfjit code");
	bpfjit_free_code(code);
}

void
test_bpf_filter(size_t counter, size_t dummy)
{
	size_t i;
	unsigned int ret = 0;

	for (i = 0; i < counter; i++) {
		ret += bpf_filter(insns, test_pkt,
		    sizeof(test_pkt), sizeof(test_pkt));
	}

	if (counter == dummy)
		printf("bpf_filter returned %u\n", ret);
}

void usage(const char *prog)
{

	fprintf(stderr,
	    "USAGE: time %s -b|-j|-c NNN\n"
	    " -b  - run bpf_filter\n"
	    " -c  - run C code\n"
	    " -j  - run bpfjit code\n"
	    " NNN - number of iterations\n", prog);
}

int main(int argc, char* argv[])
{
	int dummy;
	char cmd;
	double counter;

	dummy = (argc == INT_MAX - 1) ? argv[argc-1][0] : 1;

	if (argc == 3) {
		cmd = argv[1][1];
		counter = strtod(argv[2], NULL);
		if (counter < 0 || counter > UINT32_MAX)
			counter = 1;
	} else {
		cmd = 'j';
		counter = 1;
		usage(argv[0]);
	}

	if (!bpf_validate(insns, sizeof(insns) / sizeof(insns[0])))
		errx(EXIT_FAILURE, "Not valid bpf program");

	switch (cmd) {
	case 'j':
		test_bpfjit(counter, test_pkt, sizeof(test_pkt), dummy);
		break;
	case 'b':
		test_bpf_filter(counter, dummy);
		break;
	case 'c':
		test_c(counter, test_pkt, sizeof(test_pkt), dummy);
	}

	return EXIT_SUCCESS;
}
