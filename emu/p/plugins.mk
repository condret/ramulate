CC = gcc
CFLAGS = $(shell pkg-config --cflags r_util r_reg r_io r_bin r_asm r_anal) -Wall -g -I./../../include
LDFLAGS = $(shell pkg-config --libs r_util r_reg r_io r_bin r_asm r_anal) -L./../../ -l emu
LIBFLAGS = -shared -Wl,-soname, -fPIC

ALL_TARGETS=

ARCHS= test.mk gb.mk snes.mk

include $(ARCHS)

plugin: ${ALL_TARGETS}

clean:
	rm -f -v *.so *.o

install:
	mkdir -p /usr/lib/ramulate
	cp *.so /usr/lib/ramulate/
	chmod 755 /usr/lib/ramulate/*.so

uninstall:
	rm -Rf /usr/lib/ramulate

.PHONY: plugin clean install uninstall
