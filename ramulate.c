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
	ut32 counter=0;
	gb_init(gb);
	show_regs(gb->reg,32);
	show_regs(gb->reg,16);
	show_regs(gb->reg,8);
	show_regs(gb->reg,1);
	while(gb_step(gb) && counter!=0xff)
		counter++;
	eprintf("\n%i ops were emulated!!!\n",counter);
	show_regs(gb->reg, 16);
	r_io_close(gb->io,gb->io->fd);
	gb_emu_free(gb);
}

void gb_init(GBemu *gb)
{
	gb_reg_profile(gb);
	r_asm_setup(gb->a, "gb", 8, 0);
	gb->mbc->type = gb_get_mbc(gb->io);
	gb_sections(gb->io, gb->bin);
	ut8 wbuf=0;
	r_io_cache_write(gb->io, 0xff05, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff06, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff07, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff17, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff21, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff22, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff42, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff43, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff45, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff4a, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff4b, &wbuf, 1);
	r_io_cache_write(gb->io, 0xffff, &wbuf, 1);
	wbuf = 0x80;
	r_io_cache_write(gb->io, 0xff10, &wbuf, 1);
	wbuf = 0xbf;
	r_io_cache_write(gb->io, 0xff11, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff14, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff19, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff1e, &wbuf, 1);
	wbuf = 0xf3;
	r_io_cache_write(gb->io, 0xff12, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff25, &wbuf, 1);
	wbuf = 0x3f;
	r_io_cache_write(gb->io, 0xff16, &wbuf, 1);
	wbuf = 0x7f;
	r_io_cache_write(gb->io, 0xff1a, &wbuf, 1);
	wbuf = 0xff;
	r_io_cache_write(gb->io, 0xff1b, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff20, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff48, &wbuf, 1);
	r_io_cache_write(gb->io, 0xff49, &wbuf, 1);
	wbuf = 0x9f;
	r_io_cache_write(gb->io, 0xff1c, &wbuf, 1);
	wbuf = 0x77;
	r_io_cache_write(gb->io, 0xff24, &wbuf, 1);
	wbuf = 0xf3;
	r_io_cache_write(gb->io, 0xff25, &wbuf, 1);
	wbuf = 0xf1;
	r_io_cache_write(gb->io, 0xff26, &wbuf, 1);
	wbuf = 0x91;
	r_io_cache_write(gb->io, 0xff40, &wbuf, 1);
	wbuf = 0xfc;
	r_io_cache_write(gb->io, 0xff47, &wbuf, 1);
}
