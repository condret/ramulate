/* ramulate - LGPL - Copyright 2013 - 2014 - condret@runas-racer.com */
//#include <emu.h>
#include <r_io.h>
#include <r_reg.h>
#include <r_bin.h>
#include <r_types.h>

enum {
	GB_UMBC = -1,
	GB_ROM,
	GB_MBC1,
	GB_MBC2,
	GB_MBC3,
	GB_MBC4,
	GB_MBC5,
	GB_TAMA,
	GB_HUC3
};

enum {
	GB_NO_RAMBANKS,
	GB_1_2K_RAMBANKS,
	GB_1_8K_RAMBANKS,
	GB_4_8K_RAMBANKS,
	GB_16_8K_RAMBANKS
};

typedef struct gb_mbc_t {
	ut8 type;
	ut8 rambanks;
} GBmbc;


//new toys
GBmbc *gb_mbc_new();

//freeing
#define gb_mbc_free free

//hardware
int gb_reg_profile(emu *gb, RBin *bin);
void gb_sections(RIO *io, RBin *bin);
void gb_vsections(emu *e);
void gb_get_mbc(RIO *io, GBmbc *mbc);
//void gb_read(emu *e, ut64 addr, ut8 *buf, ut32 len);
//void gb_write(emu *e, ut64 addr, ut8 *buf, ut32 len);
int gb_step(emu *e);
// rules for reading and writing are defined here
int gb_read(emu *e, ut64 addr, ut8 *buf, ut32 len);
int gb_write(emu *e, ut64 addr, ut8 *buf, ut32 len);


// ops
int gb_push(emu *e, RReg *reg, const char *src);
int gb_pop(emu *e, RReg *reg, const char *src);

int gb_ei(RReg *reg);
int gb_di(RReg *reg);

int gb_ld_mov(RReg *reg, const char *dest, const char *src);
int gb_ld_store_const(RReg *reg, const char *dest, const ut16 src);
int gb_ld_load_to(emu *e, RReg *reg, const ut16 dest, const char *src);
int gb_ld_load_const_to(emu *e, const ut16 dest, const ut8 src);
int gb_ld_store_from(emu *e, RReg *reg, const char *dest, ut16 src);

int gb_inc(RReg *reg, const char *dest);
int gb_inc_at(emu *e, RReg *reg, const ut16 src);
int gb_dec(RReg *reg, const char *dest);

int gb_add_8_const(RReg *reg, const ut8 src);
int gb_add_8(RReg *reg, const char *src);
int gb_add_8_at(emu *e, RReg *reg, const ut16 src);
int gb_add_16(RReg *reg, const char *dest, const char *src);
int gb_adc(RReg *reg, const char *src);

int gb_sub_const(RReg *reg, const ut8 src);
int gb_sub(RReg *reg, const char *src);
int gb_sbc(RReg *reg, const char *src);

int gb_cp_const(RReg *reg, const ut8 src);
int gb_cp(RReg *reg, const char *src);
int gb_cp_at(emu *e, RReg *reg, const ut16 src);

int gb_and_const(RReg *reg, ut8 src);
int gb_and(RReg *reg, const char *src);
int gb_and_at(emu *e, RReg *reg, const ut16 src);
int gb_or_const(RReg *reg, const ut8 src);
int gb_or(RReg *reg, const char *src);
int gb_or_at(emu *e, RReg *reg, const ut16 src);
int gb_xor_const(RReg *reg, const ut8 src);
int gb_xor(RReg *reg, const char *src);
int gb_xor_at(emu *e, RReg *reg, const ut16 src);
int gb_swap(RReg *reg, const char *dest);
int gb_swap_at(emu *e, const ut16 dest);
int gb_res(RReg *reg, const ut8 arg, const char *dest);
int gb_res_at(emu *e, const ut8 arg, const ut16 dest);
int gb_set(RReg *reg, const ut8 arg, const char *dest);
int gb_set_at(emu *e, const ut8 arg, const ut16 dest);
int gb_bit(RReg *reg, const ut8 arg, const char *src);
int gb_bit_at(emu *e, RReg *reg, const ut8 arg, const ut16);
int gb_sla(RReg *reg, const char *src);
int gb_sla_at(emu *e, RReg *reg, const ut16 src);
int gb_srl(RReg *reg, const char *src);
int gb_srl_at(emu *e, RReg *reg, const ut16 src);
int gb_sra(RReg *reg, const char *src);
int gb_sra_at(emu *e, RReg *reg, const ut16 src);
int gb_rr(RReg *reg, const char *src);
int gb_rr_at(emu *e, RReg *reg, const ut16 src);
int gb_rl(RReg *reg, const char *src);
int gb_rl_at(emu *e, RReg *reg, const ut16 src);
int gb_rrc(RReg *reg, const char *src);
int gb_rlc(RReg *reg, const char *src);

int gb_jmp_rel(RReg *reg, const st8 dest);
int gb_jmp_rel_cond(RReg *reg, const char *cond, const st8 dest);
int gb_jmp(RReg* reg, const ut16 dest);
int gb_jmp_cond(RReg *reg, const char *cond, const ut16 dest);

int gb_call(emu *e, RReg *reg, const ut16 dest);
int gb_call_cond(emu *e, RReg *reg, const char *cond, const ut16 dest);

int gb_ret(emu *e, RReg *ret);
int gb_ret_cond(emu *e, RReg *reg, const char *cond);
int gb_reti(emu *e, RReg *reg);
