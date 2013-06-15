
.DEFAULT_GOAL := librabin.so

VERSION := 0.1.0
PREFIX  = /usr
LIBDIR  = $(PREFIX)/lib
INCDIR  = $(PREFIX)/include

.PHONY : clean

src/rabin.o: src/rabin.c include/rabin.h Makefile
	$(CC) -Iinclude -fPIC -g -c -Wall -Wextra $< -o $@

librabin.so.$(VERSION): src/rabin.o Makefile
	$(CC) -shared -Wl,-soname,$@ -o $@ $<

librabin.so: librabin.so.$(VERSION)
	ln -sf $< $@

clean:
	rm -- src/*.o *.so


install: librabin.so.$(VERSION)
	install -d $(LIBDIR) $(INCDIR)
	install -m 0755 librabin.so.1.0.0 $(LIBDIR)/librabin.so.1.0.0
	install -m 0644 include/rabin.h   $(INCDIR)/rabin.h
	ln -sf librabin.so.1.0.0 $(LIBDIR)/librabin.so

