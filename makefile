# Makefile generated automatically by packages.init $Revision: 1.42 $ 
# All changes will be lost at next execution!
#

OOPACKAGES = H1Examples

all: libdir bindir H1Examples.ptrs H1Examples.slib H1Examples.bin

lib: libdir bindir H1Examples.lib

profile-lib: libdir bindir H1Examples.profile-lib

slib: libdir bindir H1Examples.slib

ptrs: libdir bindir H1Examples.ptrs

bin: libdir bindir H1Examples.bin

clean: libdir bindir H1Examples.clean

clobber: libdir bindir H1Examples.clobber

depend: libdir bindir H1Examples.depend

diff:
	@for i in  $(OOPACKAGES); do \
		cd $$i; cvs diff; cd ..; \
	done

update:
	@for i in  $(OOPACKAGES); do \
		cd $$i; cvs update -A; cd ..; \
	done

nupdate:
	@for i in  $(OOPACKAGES); do \
		cd $$i; cvs -n update -A; cd ..; \
	done

libdir:
	@if [ ! -d lib ]; then mkdir lib; echo Created 'lib' directory; fi

bindir:
	@if [ ! -d bin ]; then mkdir bin; echo Created 'bin' directory; fi

H1Examples.all: H1Examples ; @cd H1Examples; gmake all;
H1Examples.lib: H1Examples ; @cd H1Examples; gmake lib;
H1Examples.profile-lib: H1Examples ; @cd H1Examples; gmake profile-lib;
H1Examples.slib: H1Examples ; @cd H1Examples; gmake slib;
H1Examples.ptrs: H1Examples ; @cd H1Examples; gmake ptrs;
H1Examples.bin: H1Examples ; @cd H1Examples; gmake bin;
H1Examples.clean: H1Examples ; @cd H1Examples; gmake clean;
H1Examples.clobber: H1Examples ; @cd H1Examples; gmake clobber;
H1Examples.depend: H1Examples ; @cd H1Examples; gmake depend;
H1Examples.diff: H1Examples ; @cd H1Examples; cvs diff;
H1Examples.update: H1Examples ; @cd H1Examples; cvs update -A;
H1Examples.nupdate: H1Examples ; @cd H1Examples; cvs -n update -A;


