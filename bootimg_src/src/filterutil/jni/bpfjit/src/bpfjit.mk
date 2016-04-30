LIBS=		libbpfjit.la
INCS=		bpfjit.h

CFLAGS=		-std=gnu99 -O2 -Wall
CFLAGS+=	-I../ -I../sljit_svn/sljit_src/
#XXX CFLAGS+=	-DBPFJIT_USE_UDIV
CFLAGS+=	-DSLJIT_CONFIG_AUTO=1
CFLAGS+=	-DSLJIT_VERBOSE=0
CFLAGS+=	-DSLJIT_DEBUG=0

LDFLAGS=        -rpath $(LIBDIR)

ILIBDIR=	$(DESTDIR)/$(LIBDIR)
IINCDIR=	$(DESTDIR)/$(INCDIR)

libbpfjit_OBJS=	bpfjit.lo ../sljit_svn/sljit_src/sljitLir.lo

all: libbpfjit.la

%.lo: %.c
	libtool --mode=compile --tag CC $(CC) $(CFLAGS) -c $<

libbpfjit.la: $(libbpfjit_OBJS)
	libtool --mode=link --tag CC $(CC) $(LDFLAGS) -o $@ $(notdir $^)

install/%.la: %.la
	mkdir -p $(ILIBDIR)
	libtool --mode=install install -c $(notdir $@) $(ILIBDIR)/$(notdir $@)

install: $(addprefix install/,$(LIBS))
	libtool --mode=finish $(LIBDIR)
	mkdir -p $(IINCDIR)
	install -c $(INCS) $(IINCDIR)

clean:
	libtool --mode=clean rm
	@ rm -rf .libs *.o *.lo *.la
