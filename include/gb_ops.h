#include <r_asm.h>
#include <r_io.h>
#include <r_reg.h>

int gb_ld_mov(RReg *reg, const char *dest, const char *src);
int gb_ldi_mov(RReg *reg, const char *dest, const char *src);
int gb_ldd_mov(RReg *reg, const char *dest, const char *src);
int gb_ld_store_const(RReg *reg, const char *dest, const ut16 src);
int gb_inc(RReg *reg, const char *dest);
int gb_dec(RReg *reg, const char *dest);
int gb_swap_reg(RReg *reg, const char *dest);
int gb_jmp_rel(RReg *reg, const st8 dest);
