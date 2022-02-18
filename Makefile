CC=cc
CFLAGS=-I. -Iinclude
LDFLAGS=-L.
PREFIX=/usr/local
SONAME=0

all: cleanpath

config.h: config.h.in
	rm -f config.h
    # Broad assumption. Chances are you have it though if you're using this makefile instead of cmake
	sed 's/cmakedefine/define/;s/@HAVE_CONFSTR@/1/' config.h.in > config.h.tmp
	mv config.h.tmp config.h

main.o: config.h
	$(CC) -c $(CFLAGS) $(LDFLAGS) src/main.c

cleanpath.o: config.h
	$(CC) -c $(CFLAGS) $(LDFLAGS) -fPIC lib/cleanpath.c

libcleanpath.so: cleanpath.o
	$(CC) -o libcleanpath.so.$(SONAME) -fPIC -shared cleanpath.o
	ln -s -f libcleanpath.so.$(SONAME) libcleanpath.so

libcleanpath.a: cleanpath.o
	ar -crs libcleanpath.a cleanpath.o

cleanpath: main.o libcleanpath.a libcleanpath.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o cleanpath main.o libcleanpath.a

install: cleanpath
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/include/cleanpath
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	mkdir -p $(DESTDIR)$(PREFIX)/share/man1
	mkdir -p $(DESTDIR)$(PREFIX)/share/man3
	install -m 755 cleanpath $(DESTDIR)$(PREFIX)/bin
	install -m 755 libcleanpath.so $(DESTDIR)$(PREFIX)/lib
	install -m 755 libcleanpath.so.$(SONAME) $(DESTDIR)$(PREFIX)/lib
	install -m 644 include/cleanpath/cleanpath.h $(DESTDIR)$(PREFIX)/include/cleanpath
	install -m 644 docs/man/cleanpath.1 $(DESTDIR)$(PREFIX)/share/man1
	install -m 644 docs/man/cleanpath_filter.3 $(DESTDIR)$(PREFIX)/share/man1

.PHONY:
clean:
	rm -f cleanpath
	rm -f *.o
	rm -f config.h
	rm -f libcleanpath.a
	rm -f libcleanpath.so*
