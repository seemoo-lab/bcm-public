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

#include "bpfjit.h"

#include <stddef.h>
#include <stdint.h>

unsigned int filter_pkt(const bpf_ctx_t *, bpf_args_t *);

/* Copied from <sys/endian.h> */
static __inline uint16_t
my_be16dec(const void *buf)
{
	const uint8_t *p = (const uint8_t *)buf;

	return ((p[0] << 8) | p[1]);
}

/* Copied from <sys/endian.h> */
static inline uint32_t
my_be32dec(const void *buf)
{
	const uint8_t *p = (const uint8_t *)buf;

	return ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
}


/*
 * C code for the following filter program:
 *
 * BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12),
 * BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x800, 0, 8),
 * BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 26),
 * BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 2),
 * BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 30),
 * BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 3, 4),
 * BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x80037023, 0, 3),
 * BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 30),
 * BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x8003700f, 0, 1),
 * BPF_STMT(BPF_RET+BPF_K, UINT32_MAX),
 * BPF_STMT(BPF_RET+BPF_K, 0),
 */
unsigned int
filter_pkt(const bpf_ctx_t *ctx, bpf_args_t *args)
{
	const uint8_t *pkt = args->pkt;
	size_t buflen  = args->buflen;
	uint32_t x;

	if (buflen < 14)
		return 0;

	if (my_be16dec(pkt + 12) != 0x800)
		return 0;

	if (buflen < 30)
		return 0;

	x = my_be32dec(pkt + 26);

	if (x == 0x8003700f) {
		if (buflen < 34)
			return 0;

		if (my_be32dec(pkt + 30) == 0x80037023)
			return UINT32_MAX;
		else
			return 0;
	} else if (x == 0x80037023) {
		if (buflen < 34)
			return 0;
		if (my_be32dec(pkt + 30) == 0x8003700f)
			return UINT32_MAX;
		else
			return 0;
	} else {
		return 0;
	}
}
