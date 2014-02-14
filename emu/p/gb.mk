GBOBJ = gb.o gb_ops.o emu_gb.o
TARGET_GB=emu_gb.so
ALL_TARGETS+=${TARGET_GB}

${TARGET_GB}: $(GBOBJ)
	${CC} ${LDFLAGS} ${GBOBJ} ${LIBFLAGS} -o ${TARGET_GB}

gb.o: ../arch/gb/gb.c
	${CC} ${CFLAGS} -fPIC -I./../arch/include ../arch/gb/gb.c -c -o gb.o

gb_ops.o: ../arch/gb/gb_ops.c
	$(CC) $(CFLAGS) -fPIC -I./../arch/include ../arch/gb/gb_ops.c -c -o gb_ops.o

emu_gb.o: emu_gb.c
	$(CC) $(CFLAGS) -fPIC -I./../arch/include emu_gb.c -c -o emu_gb.o

.PHONY: ${TARGET_GB} $(GBOBJ)
