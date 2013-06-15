
.DEFAULT_GOAL := librabin.so

src/rabin.o: src/rabin.c include/rabin.h Makefile
	$(CC) -Iinclude -fPIC -g -c -Wall -Wextra $< -o $@

librabin.so.1.0.0: src/rabin.o Makefile
	$(CC) -shared -Wl,-soname,$@ -o $@ $<

librabin.so: librabin.so.1.0.0
	ln -sf $< $@


PREFIX = /usr
LIBDIR = $(PREFIX)/lib
INCDIR = $(PREFIX)/include

install: librabin.so.1.0.0
	install -d $(LIBDIR) $(INCDIR)
	install -m 0755 librabin.so.1.0.0 $(LIBDIR)/librabin.so.1.0.0
	install -m 0644 include/rabin.h   $(INCDIR)/rabin.h
	ln -sf librabin.so.1.0.0 $(LIBDIR)/librabin.so

