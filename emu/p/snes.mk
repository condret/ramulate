TARGET_SNES=emu_snes.so
ALL_TARGETS+=${TARGET_SNES}

${TARGET_SNES}:
	${CC} ${CFLAGS} ${LDFLAGS} emu_snes.c ${LIBFLAGS} -o ${TARGET_SNES}

.PHONY: ${TARGET_SNES}
