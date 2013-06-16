
.DEFAULT_GOAL := librabin.so

VERSION := 0.1.1
PREFIX  = /usr
LIBDIR  = $(PREFIX)/lib
INCDIR  = $(PREFIX)/include

.PHONY : clean dist

src/rabin.o: src/rabin.c include/rabin.h Makefile
	$(CC) -Iinclude -fPIC -g -c -Wall -Wextra $< -o $@

librabin.so.$(VERSION): src/rabin.o Makefile
	$(CC) -shared -Wl,-soname,$@ -o $@ $<

librabin.so: librabin.so.$(VERSION)
	ln -sf $< $@

clean:
	rm -f -- src/*.o *.so *.so.* librabin-*.tar.gz


install: librabin.so.$(VERSION)
	install -d $(LIBDIR) $(INCDIR)
	install -m 0755 librabin.so.$(VERSION) $(LIBDIR)/librabin.so.$(VERSION)
	install -m 0644 include/rabin.h   $(INCDIR)/rabin.h
	ln -sf librabin.so.$(VERSION) $(LIBDIR)/librabin.so

dist: librabin-$(VERSION).tar.gz


librabin-$(VERSION).tar.gz: src/*.c include/*.h Makefile README.md LICENSE
	ln -s . librabin-$(VERSION)
	tar czf $@ $(addprefix librabin-$(VERSION)/,$^)
	unlink librabin-$(VERSION)
