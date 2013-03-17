
src/rabin.o: src/rabin.c include/rabin.h Makefile
	$(CC) -Iinclude -fPIC -g -c -Wall -Wextra $< -o $@

librabin.so.1.0.0: src/rabin.o Makefile
	$(CC) -shared -Wl,-soname,$@ -o $@ $<

librabin.so: librabin.so.1.0.0
	ln -s $< $@
