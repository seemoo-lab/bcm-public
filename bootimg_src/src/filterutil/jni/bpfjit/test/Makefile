PROG=	bpfjit_test
SRCS=	main.c util.c test_empty.c test_ld.c \
	test_ldx.c test_alu.c test_misc.c test_jmp.c \
	test_st.c test_stx.c test_opt.c test_extmem.c \
	test_cop.c test_cop_extmem.c \
	test_copx.c test_copx_extmem.c

WARNS=	4

COPTS+=		-O2 -g
CPPFLAGS+=	-I ../src -I ../sljit/sljit_src/
CPPFLAGS+=	-DSLJIT_CONFIG_AUTO=1

LDADD+=		-lpcap -lbpfjit -lsljit
LDFLAGS+=	-L ${.OBJDIR}/../src
LDFLAGS+=	-L ${.OBJDIR}/../sljit/sljit_src

.include <mkc.prog.mk>
