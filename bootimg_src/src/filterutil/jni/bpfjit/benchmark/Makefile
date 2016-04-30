PROG=	bpfjit_benchmark
SRCS=	benchmark.c c.c

WARNS=	4

COPTS+=		-O2 -g
CPPFLAGS+=	-I ../src -I ../sljit/sljit_src/
CPPFLAGS+=	-DSLJIT_CONFIG_AUTO=1

LDADD+=		-lpcap -lbpfjit -lsljit
LDFLAGS+=	-L ${.OBJDIR}/../src
LDFLAGS+=	-L ${.OBJDIR}/../sljit/sljit_src

.include <mkc.prog.mk>
