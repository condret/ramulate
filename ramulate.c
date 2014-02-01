/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <r_reg.h>
#include <r_io.h>
#include <r_bin.h>
#include <r_asm.h>
#include <r_util.h>
#include <emu.h>
#include <gb.h>
#include <stdlib.h>

void print_vs(emu *e)
{
	VSection *vs;
	RListIter *iter;
	r_list_foreach(e->vsections, iter, vs) {
		eprintf("id: [%i]\t\tname: vsection.%s\t\taddr: 0x%"PFMT64x"\t\tsize:0x%"PFMT64x"\tPerms: %s\t",
			vs->id, vs->name, vs->addr, vs->size, r_str_rwx_i(vs->rwx));
		switch(vs->linked) {
			case VS_NO_LINK:
				eprintf("not linked\n");
				break;
			case VS_IS_LINKED:
				eprintf("is linked to vsection.%s\n", virtual_section_resolve_link(e, vs)->name);
				break;
			case VS_HAS_LINKED:
				eprintf("has linked\n");
				break;
		}
	}
}

int main(int argc, char *argv[])
{
	if(argc<2) {
		eprintf("which rom?\n");
		return R_FALSE;
	}
	emu *e = emu_new();
	e->io->fd = r_io_open(e->io, argv[1], R_IO_READ, 0);
	if(!e->io->fd) {
		eprintf("Wrong path?\n");
		emu_free(e);
		return R_FALSE;
	}
	RBin *bin = r_bin_new();
	r_bin_load(bin, argv[1], 0);
	if(!(	bin->cur.o->info->arch[0]=='g' &&				//pastafari
		bin->cur.o->info->arch[1]=='b' &&
		bin->cur.o->info->arch[2]==0)) {
			eprintf("this is not a gb-rom\n");
			r_io_close(e->io, e->io->fd);
			r_bin_free(bin);
			emu_free(e);
			return R_FALSE;
	}
	r_asm_setup(e->a, "gb", 8, 0);
	gb_sections(e->io, bin);
	gb_reg_profile(e, bin);
	r_bin_free(bin);
	e->data = gb_mbc_new();
	gb_get_mbc(e->io, e->data);
	gb_vsections(e);
	while(r_reg_getv(e->reg, "mpc")<0x10000)
		gb_step(e);
	gb_mbc_free(e->data);
	r_io_close(e->io,e->io->fd);
	emu_free(e);
	return R_TRUE;
}
