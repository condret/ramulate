/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <r_reg.h>

int gb_push(void *e, RReg *reg, const char *src);
int gb_pop(void *, RReg *reg, const char *src);

int gb_ei(RReg *reg);
int gb_di(RReg *reg);

int gb_ld_mov(RReg *reg, const char *dest, const char *src);
int gb_ld_store_const(RReg *reg, const char *dest, const ut16 src);
int gb_ld_load_to(void *e, RReg *reg, const ut16 dest, const char *src);
int gb_ld_store_from(void *e, RReg *reg, const char *dest, ut16 src);

int gb_inc(RReg *reg, const char *dest);
int gb_dec(RReg *reg, const char *dest);

int gb_add_8(RReg *reg, const char *src);
int gb_add_8_at(void *e, RReg *reg, const ut16 src);
int gb_adc(RReg *reg, const char *src);

int gb_sub_const(RReg *reg, const ut8 src);
int gb_sub(RReg *reg, const char *src);
int gb_sbc(RReg *reg, const char *src);

int gb_cp_const(RReg *reg, const ut8 src);
int gb_cp(RReg *reg, const char *src);

int gb_and(RReg *reg, const char *src);
int gb_and_at(void *e, RReg *reg, const ut16 src);
int gb_xor_const(RReg *reg, const ut8 src);
int gb_xor(RReg *reg, const char *src);
int gb_swap(RReg *reg, const char *dest);
int gb_swap_at(void *e, const ut16 dest);
int gb_res(RReg *reg, const ut8 arg, const char *dest);
int gb_res_at(void *e, const ut8 arg, const ut16 dest);
int gb_set(RReg *reg, const ut8 arg, const char *dest);
int gb_set_at(void *e, const ut8 arg, const ut16 dest);
int gb_bit(RReg *reg, const ut8 arg, const char *src);
int gb_bit_at(void *e, RReg *reg, const ut8 arg, const ut16);
int gb_sla(RReg *reg, const char *src);
int gb_sla_at(void *e, RReg *reg, const ut16 src);
int gb_srl(RReg *reg, const char *src);
int gb_srl_at(void *e, RReg *reg, const ut16 src);
int gb_sra(RReg *reg, const char *src);
int gb_sra_at(void *e, RReg *reg, const ut16 src);
int gb_rr(RReg *reg, const char *src);
int gb_rr_at(void *e, RReg *reg, const ut16 src);
int gb_rl(RReg *reg, const char *src);
int gb_rl_at(void *e, RReg *reg, const ut16 src);

int gb_jmp_rel(RReg *reg, const st8 dest);
int gb_jmp_rel_cond(RReg *reg, const char *cond, const st8 dest);
int gb_jmp(RReg* reg, const ut16 dest);
int gb_jmp_cond(RReg *reg, const char *cond, const ut16 dest);

int gb_call(void *e, RReg *reg, const ut16 dest);
int gb_call_cond(void *e, RReg *reg, const char *cond, const ut16 dest);

int gb_ret(void *e, RReg *ret);
int gb_ret_cond(void *e, RReg *reg, const char *cond);
int gb_reti(void *e, RReg *reg);
