/* ramulate - LGPL - Copyright 2013 - 2014 - condret@runas-racer.com */

#include <r_reg.h>
#include <r_io.h>
#include <r_bin.h>
#include <r_lib.h>
#include <r_asm.h>
#include <r_util.h>
#include <r_anal.h>
#include <emu.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

inline void set_sections (RIO *io, RBin *bin);
static int emulation (emu *e, ut8 *buf);

int main(int argc, char *argv[])
{
	emu *e;
	ut8 *buf;
	char *file = NULL, *arch = NULL;
	int opt;
	ut32 c = 0;
	switch (argc) {
		case 1:
			break;
		case 2:
			file = argv[1];
			break;
		default:
			while ((opt = getopt(argc, argv, "a:f:")) != -1) {
				switch (opt) {
					case 'a':
						arch = optarg;
						break;
					case 'f':
						file = optarg;
						break;
				}
			}
			break;
	}

	if (!file) {
		eprintf ("Which file\n");
		return R_FALSE;
	}

	e = emu_new();
	e->io->fd = r_io_open (e->io, file, R_IO_READ, 0);
	if (!e->io->fd) {
		eprintf ("Wrong Path\n");
		emu_free(e);
		return R_FALSE;
	}

	r_bin_load (e->bin, argv[1], 0, 0, 0);
	if (!e->bin->cur->o->info && !arch) {
		eprintf("No such bin plugin\n");
		r_io_close (e->io, e->io->fd);
		emu_free(e);
		return R_FALSE;
	}

	if (!arch)
		arch = e->bin->cur->o->info->arch;

	r_lib_opendir (e->lib, "/lib/ramulate");
	emu_use (e, arch);

	if(!e->plugin) {
		eprintf ("No such emu plugin for %s\nBut we have these:\n", arch);
		emu_list_plugins (e);
		emu_free(e);
		return R_FALSE;
	}

	printf ("EMU:\t%s\t%s\n",e->plugin->arch, e->plugin->desc);

	if (r_asm_use (e->a, e->plugin->arch)) {
		printf("R_ASM:\t%s\n", e->plugin->arch);
	} else {
		printf("R_ASM:\tNONE\n");
		if (e->plugin->deps & EMU_PLUGIN_DEP_ASM) {					//check asm-plugin-dependencies
			eprintf("Missing asm-plugin-dependency, please update your r2-version\n");
			r_io_close (e->io, e->io->fd);
			emu_free (e);
			return R_FALSE;
		}
	}


	if (r_anal_use (e->anal, e->plugin->arch)) {						//check anal-plugin-dependecies
		printf("R_ANAL:\t%s\n", e->plugin->arch);
	} else {
		printf("R_ANAL:\tNONE\n");
		if (e->plugin->deps & EMU_PLUGIN_DEP_ANAL) {
			eprintf("Missing anal-plugin-dependency, please update your r2-version\n");
			r_io_close (e->io, e->io->fd);
			emu_free (e);
		}
	}

	if (e->plugin->allocate_data && e->plugin->free_data)
		e->plugin->allocate_data (e);

	set_sections (e->io, e->bin);

	if (e->plugin->set_reg_profile)
		e->plugin->set_reg_profile (e);

	if (e->plugin->set_vs_profile)
		e->plugin->set_vs_profile (e);

	if (e->plugin->min_read_sz)
		buf = malloc (e->plugin->min_read_sz);
	else	buf = malloc (sizeof(int));							//LOOOOOOOOOL :D :D :D

	if (e->plugin->step)
		while ( c < 10000 && emulation (e, buf)) c++;
	else printf ("cannot emulate, please check that plugin\n");


	free (buf);

	if (e->plugin->allocate_data && e->plugin->free_data)
		e->plugin->free_data (e->data);
	r_io_close (e->io, e->io->fd);

	printf("%s\n", virtual_section_get_addr(e, 0xe000)->name);
	emu_free (e);
	return R_TRUE;
}


inline void set_sections(RIO *io, RBin *bin)
{
	RBinSection *section;
	RListIter *iter;
	RList *sections = r_bin_get_sections (bin);
	r_list_foreach(sections, iter, section) {
		r_io_section_add (io,	section->offset, section->rva, section->size,
					section->vsize, section->srwx, section->name);
	}
}


static int emulation (emu *e, ut8 *buf)
{
	ut64 addr = r_reg_getv (e->reg, r_reg_get_name (e->reg, R_REG_NAME_PC));		//Check Breakboints here: new return stat for that
	if (e->plugin->read) {
		if (e->plugin->min_read_sz)
			e->plugin->read (e, addr, buf, e->plugin->min_read_sz);
		else	e->plugin->read (e, addr, buf, sizeof(int));
	} else {
		if (e->plugin->min_read_sz)
			emu_read (e, addr, buf, e->plugin->min_read_sz);
		else	emu_read (e, addr, buf, sizeof(int));
	}

	if (e->plugin->deps & EMU_PLUGIN_DEP_ASM) {						//only disassemble if it is necessary
		r_asm_set_pc (e->a, addr);
		if (e->plugin->min_read_sz)
			r_asm_disassemble (e->a, e->op, buf, e->plugin->min_read_sz);
		else	r_asm_disassemble (e->a, e->op, buf, sizeof(int));
	}

	if (e->plugin->deps & EMU_PLUGIN_DEP_ANAL) {						//only analize if it is necessary
		if (e->plugin->min_read_sz)
			r_anal_op (e->anal, e->anop, addr, buf, e->plugin->min_read_sz);
		else	r_anal_op (e->anal, e->anop, addr, buf, sizeof(int));
	}

	return e->plugin->step (e, buf);
}
