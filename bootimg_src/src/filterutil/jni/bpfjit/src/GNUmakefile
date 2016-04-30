# GNU Makefile for Travis CI. For normal builds, use mkcmake.
CFLAGS+=	-I../ -I../sljit/sljit_src/
#XXX CFLAGS+=	-DBPFJIT_USE_UDIV
CFLAGS+=	-DSLJIT_CONFIG_AUTO=1
CFLAGS+=	-DSLJIT_VERBOSE=0
CFLAGS+=	-DSLJIT_DEBUG=0

LIB_A=	libbpfjit.a

AR=     ar rcu
RANLIB= ranlib
RM=     rm -f

OBJS=	bpfjit.o

all: libbpfjit
libbpfjit: ${LIB_A}

${LIB_A}: ${OBJS}
	${AR} $@ ${OBJS}
	${RANLIB} $@
