/* ramulate - LGPL - Copyright 2013 - 2014 - condret@runas-racer.com */
#ifndef EMU_GB_H
#define EMU_GB_H

#include <emu.h>
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

typedef struct gb_cpu_stats_t {
	GBmbc *mbc;
	ut32 cycles;
	ut8 input;
	ut8 prev_input;
	ut16 interruptlevel;
} GBCpuStats;

//new toys
GBmbc *gb_mbc_new (emu *e);
GBCpuStats *gb_cpu_stats_new (emu *e);
void gb_data_new (emu *e);

//freeing
#define gb_mbc_free free
void gb_cpu_stats_free (GBCpuStats *cpu);
void gb_data_free (void *data);

//hardware
int gb_set_reg_profile(emu *gb);
int gb_set_vs_profile(emu *e);
void gb_get_mbc(RIO *io, GBmbc *mbc);
//void gb_read(emu *e, ut64 addr, ut8 *buf, ut32 len);
//void gb_write(emu *e, ut64 addr, ut8 *buf, ut32 len);
int gb_step(emu *e, ut8 *buf);

// rules for reading and writing are defined here
int gb_read(emu *e, ut64 addr, ut8 *buf, ut32 len);
int gb_write(emu *e, ut64 addr, ut8 *buf, ut32 len);

//ops
int gb_mov (emu *e);
int gb_xor (emu *e);
int gb_add (emu *e);
int gb_cp (emu *e);
int gb_sub (emu *e);
int gb_jmp (emu *e);
int gb_cjmp (emu *e);
int gb_ujmp (emu *e);
int gb_call (emu *e);
int gb_ccall (emu *e);
int gb_ret (emu *e);
int gb_cret (emu *e);
int gb_store (emu *e);
int gb_load (emu *e);

#endif
