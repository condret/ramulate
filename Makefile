CC = gcc
CFLAGS = $(shell pkg-config --cflags r_reg r_io r_asm r_bin r_anal r_util) -I./include -Wall -g
LDFLAGS = $(shell pkg-config --libs r_reg r_io r_asm r_bin r_anal r_util) -L./ -l emu
LIBFLAGS = -shared -Wl,-soname,
LOBJ = emu.o vsection.o e_io.o plugins.o
LIB0 = lib

all: ramulate
	make -C emu/p/ -f plugins.mk plugin

ramulate: $(LIB0)
	$(CC) $(CFLAGS) $(LDFLAGS) -o ramulate ramulate.c $(OBJ)

lib: $(LOBJ)
	$(CC) $(LIBFLAGS) $(LOBJ) -o libemu.so

emu.o: emu/emu.c
	$(CC) -fPIC $(CFLAGS) -c emu/emu.c

vsections.o: emu/vsections.c
	$(CC) -fPIC $(CFLAGS) -c emu/vsections.c

e_io.o: emu/e_io.c
	$(CC) -fPIC $(CFLAGS) -c emu/e_io.c

plugins.o: emu/plugins.c
	$(CC) -fPIC $(CFLAGS) -c emu/plugins.c

clean:
	rm -f *.o *.so ramulate
	make -C emu/p/ -f plugins.mk clean

install:
	cp libemu.so /usr/lib
	ldconfig
	chmod 755 /usr/lib/libemu.so
	cp ramulate /usr/bin
	chmod 755 /usr/bin/ramulate
	make -C emu/p/ -f plugins.mk install

uninstall:
	rm /usr/lib/libemu.so /usr/bin/ramulate
	ldconfig
	make -C emu/p/ -f plugins.mk uninstall

.PHONY: ramulate lib ${LOBJ} clean install uninstall all
