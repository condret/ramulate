/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <r_io.h>
#include <r_reg.h>

int gb_push(RIO *io, RReg *reg, const char *src);
int gb_pop(RIO *io, RReg *reg, const char *src);

int gb_ei(RReg *reg);
int gb_di(RReg *reg);

int gb_ld_mov(RReg *reg, const char *dest, const char *src);
int gb_ldi_mov(RReg *reg, const char *dest, const char *src);
int gb_ldd_mov(RReg *reg, const char *dest, const char *src);
int gb_ld_store_const(RReg *reg, const char *dest, const ut16 src);
int gb_ld_load_to(RIO *io, RReg *reg, const ut16 dest, const char *src);
int gb_ld_store_from(RIO *io, RReg *reg, const char *dest, const ut16 src);

int gb_inc(RReg *reg, const char *dest);
int gb_dec(RReg *reg, const char *dest);

int gb_add_8(RReg *reg, const char *src);
int gb_add_8_at(RIO *io, RReg *reg, const ut16 src);
int gb_adc(RReg *reg, const char *src);

int gb_sub_const(RReg *reg, const ut8 src);
int gb_sub(RReg *reg, const char *src);
int gb_sbc(RReg *reg, const char *src);

int gb_cp_const(RReg *reg, const ut8 src);
int gb_cp(RReg *reg, const char *src);

int gb_and(RReg *reg, const char *src);
int gb_and_at(RIO *io, RReg *reg, const ut16 src);
int gb_xor_const(RReg *reg, const ut8 src);
int gb_xor(RReg *reg, const char *src);
int gb_swap(RReg *reg, const char *dest);
int gb_swap_at(RIO *io, const ut16 dest);
int gb_res(RReg *reg, const ut8 arg, const char *dest);
int gb_res_at(RIO *io, const ut8 arg, const ut16 dest);
int gb_set(RReg *reg, const ut8 arg, const char *dest);
int gb_set_at(RIO *io, const ut8 arg, const ut16 dest);
int gb_bit(RReg *reg, const ut8 arg, const char *src);
int gb_bit_at(RIO *io, RReg *reg, const ut8 arg, const ut16);
int gb_sla(RReg *reg, const char *src);
int gb_sla_at(RIO *io, RReg *reg, const ut16 src);
int gb_srl(RReg *reg, const char *src);
int gb_srl_at(RIO *io, RReg *reg, const ut16 src);
int gb_sra(RReg *reg, const char *src);
int gb_sra_at(RIO *io, RReg *reg, const ut16 src);
int gb_rr(RReg *reg, const char *src);

int gb_jmp_rel(RReg *reg, const st8 dest);
int gb_jmp_rel_cond(RReg *reg, const char *cond, const st8 dest);
int gb_jmp(RReg* reg, const ut16 dest);
int gb_jmp_cond(RReg *reg, const char *cond, const ut16 dest);

int gb_call(RIO *io, RReg *reg, const ut16 dest);
int gb_call_cond(RIO *io, RReg *reg, const char *cond, const ut16 dest);

int gb_ret(RIO *io, RReg *ret);
int gb_ret_cond(RIO *io, RReg *reg, const char *cond);
int gb_reti(RIO *io, RReg *reg);
