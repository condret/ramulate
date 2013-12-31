/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <r_reg.h>
#include <r_bin.h>
#include <r_io.h>
#include <r_asm.h>
#include <r_list.h>
#include <string.h>
#include <gb_ops.h>

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

typedef struct gb_mbc_t {
	ut8 type;
} GBmbc;

typedef struct gb_emu_t {
	RReg *reg;
	RIO *io;
	RBin *bin;
	RAsm *a;
	RAsmOp *op;
	GBmbc *mbc;
} GBemu;

//new toys
GBemu *gb_emu_new();
GBmbc *gb_mbc_new();

//freeing
void gb_emu_free(GBemu *gb);
#define gb_mbc_free free

//setup & helpers
void gb_sections(RIO *io, RBin *bin);
int gb_reg_profile(GBemu *gb);
st8 gb_get_mbc(RIO *io);

// testing area
void show_regs(struct r_reg_t *reg, int bitsize);

// execution
int gb_step(GBemu *gb);
