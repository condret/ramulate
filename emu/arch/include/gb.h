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

enum {
	GB_INT_VBL = 0x40,		//VBlank
	GB_INT_LCD = 0x48,		//LCDC-Status
	GB_INT_TIM = 0x50,		//Timer-overflow
	GB_INT_STC = 0x58,		//Serial-Transfer-Completition
	GB_INT_JP  = 0x60		//Joypad
};

typedef struct gb_mbc_t {
	ut8 type;
	ut8 rambanks;
} GBmbc;

typedef struct gb_cpu_stats_t {
	GBmbc *mbc;
	ut32 cycles;			//timer
	ut32 prev_cycles;		//timer
	ut8 input;			//JOYPAD
	ut8 prev_input;			//JOYPAD
	ut16 interruptlevel;		//level of interrupt, needed for ret and reti
	ut8 div;
	ut8 tima;
	ut8 tac;
	ut32 timer_ccl;			//number of cycles since last timer-increment
	ut8 ly;
	ut16 render_ccl;		//missing ccl until ly gets incremented
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
int gb_input_handler (emu *e, char input);
int gb_step(emu *e, ut8 *buf);

// rules for reading and writing are defined here
int gb_read(emu *e, ut64 addr, ut8 *buf, ut32 len);
int gb_write(emu *e, ut64 addr, ut8 *buf, ut32 len);

//ops
int gb_mov (emu *e);
int gb_push (emu *e);
int gb_pop (emu *e);
int gb_xor (emu *e);
int gb_and (emu *e);
int gb_or (emu *e);
int gb_bit (emu *e);
int gb_rol (emu *e);
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
