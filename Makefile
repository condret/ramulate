CC = gcc
CFLAGS = $(shell pkg-config --cflags r_reg r_io r_bin r_asm) -I./include
LDFLAGS = $(shell pkg-config --libs r_reg r_io r_bin r_asm)
OBJ = gb_emu.o gb_ops.o gb_tests.o

ramulate: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o ramulate ramulate.c $(OBJ)

gb_emu.o: gb_emu/gb_emu.c
	$(CC) $(CFLAGS) -c gb_emu/gb_emu.c

gb_ops.o: gb_emu/gb_ops.c
	$(CC) $(CFLAGS) -c gb_emu/gb_ops.c

gb_tests.o: gb_emu/gb_tests.c
	$(CC) $(CFLAGS) -c gb_emu/gb_tests.c

clean:
	rm -f *.o ramulate
