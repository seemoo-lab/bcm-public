/*-
 * Copyright (c) 2011 Alexander Nasonov.
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

#ifndef BPFJIT_TEST_UTIL_H
#define BPFJIT_TEST_UTIL_H

#include "bpfjit.h"

#include <err.h>
#include <stdlib.h>

extern int exit_status;

#define REQUIRE(x) if (!(x)) { \
	errx(EXIT_FAILURE, "%s:%u (in %s): %s\nAborted",  \
	    __FILE__, __LINE__, __func__, #x); }

#define CHECK(x) if (!(x)) { \
	exit_status = EXIT_FAILURE; \
	warnx("%s:%u (in %s): %s", __FILE__, __LINE__, __func__, #x); }

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

#endif /* #ifndef BPFJIT_TEST_UTIL_H */
