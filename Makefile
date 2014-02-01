CC = gcc
CFLAGS = $(shell pkg-config --cflags r_reg r_io r_asm r_bin) -I./include -Wall -g
LDFLAGS = $(shell pkg-config --libs r_reg r_io r_asm r_bin) -L./ -l emu
LIBFLAGS = -shared -Wl,-soname,
OBJ = gb.o gb_ops.o
LOBJ = emu.o vsection.o e_io.o
LIB0 = lib

ramulate: $(OBJ) $(LIB0)
	$(CC) $(CFLAGS) $(LDFLAGS) -o ramulate ramulate.c $(OBJ)

lib: $(LOBJ)
	$(CC) $(LIBFLAGS) $(LOBJ) -o libemu.so

emu.o: emu/emu.c
	$(CC) -fPIC $(CFLAGS) -c emu/emu.c

vsection.o: emu/vsection.c
	$(CC) -fPIC $(CFLAGS) -c emu/vsection.c

e_io.o: emu/e_io.c
	$(CC) -fPIC $(CFLAGS) -c emu/e_io.c

gb.o: arch/gb/gb.c
	$(CC) $(CFLAGS) -c arch/gb/gb.c

gb_ops.o: arch/gb/gb_ops.o
	$(CC) $(CFLAGS) -c arch/gb/gb_ops.c -Ofast

clean:
	rm -f *.o *.so ramulate


install:
	cp libemu.so /usr/lib
	ldconfig
	chmod 755 /usr/lib/libemu.so
	cp ramulate /usr/bin
	chmod 755 /usr/bin/ramulate

uninstall:
	rm /usr/lib/libemu.so /usr/bin/ramulate
	ldconfig
