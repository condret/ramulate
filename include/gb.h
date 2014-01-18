/* ramulate - LGPL - Copyright 2013 - 2014 - condret@runas-racer.com */
#include <emu.h>
#include <r_reg.h>
#inlcude <r_types.h>

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
	ut8 rambanks;
} GBmbc;


//new toys
GBmbc *gb_mbc_new();

//freeing
#define gb_mbc_free free

//hardware
void gb_reg_profile(RReg *reg);
void gb_sections(emu *e);
void gb_vsections(emu *e);
void gb_read(emu *e, ut64 addr, ut8 *buf, ut32 len);
void gb_write(emu *e, ut64 addr, ut8 *buf, ut32 len);
void gb_step(emu *e);
// rules for reading and writing are defined here
