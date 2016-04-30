rpm: sljit_svn
	mkdir -p SOURCES && tar czpvf SOURCES/libbpfjit.tar.gz src sljit_svn
	rpmbuild -ba -v --define "_topdir ${PWD}" SPECS/libbpfjit.spec

sljit_svn:
	svn co svn://svn.code.sf.net/p/sljit/code@r220 sljit_svn

clean:
	make -C src -f bpfjit.mk clean
	rm -rf BUILD BUILDROOT RPMS SOURCES SRPMS

.PHONY: rpm clean
