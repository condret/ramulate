/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <r_reg.h>
#include <r_io.h>
#include <r_bin.h>
#include <r_asm.h>
#include <gb_emu.h>

void gb_init(GBemu *gb);

void main(int argc, char *argv[])
{
	if(argc<2) {
		eprintf("which rom?\n");
		return;
	}
	GBemu *gb = gb_emu_new();
	gb->io->fd = r_io_open(gb->io, argv[1], R_IO_READ, 0);
	if(!gb->io->fd) {
		eprintf("Wrong path?\n");
		gb_emu_free(gb);
		return;
	}
	r_bin_load(gb->bin, argv[1], 0);
	if(!(	gb->bin->cur.o->info->arch[0]=='g' &&				//pastafari
		gb->bin->cur.o->info->arch[1]=='b' &&
		gb->bin->cur.o->info->arch[2]==0)) {
			eprintf("this is not a gb-rom\n");
			r_io_close(gb->io, gb->io->fd);
			gb_emu_free(gb);
			return;
	}
	gb_init(gb);
	show_regs(gb->reg,32);
	show_regs(gb->reg,16);
	show_regs(gb->reg,8);
	show_regs(gb->reg,1);
	r_reg_set_value(gb->reg, r_reg_get(gb->reg, "mpc", -1), 0x243);
	show_regs(gb->reg, 16);
	gb_step(gb);
	show_regs(gb->reg, 16);
	gb_step(gb);
	show_regs(gb->reg, 16);
	r_io_close(gb->io,gb->io->fd);
	gb_emu_free(gb);
}

void gb_init(GBemu *gb)
{
	gb_reg_profile(gb);
	r_asm_setup(gb->a, "gb", 8, 0);
	gb->mbc->type = gb_get_mbc(gb->io);
}
