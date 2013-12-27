#include <r_reg.h>
#include <r_io.h>
#include <r_bin.h>
#include <r_asm.h>
#include <gb_emu.h>
#include <gb_ops.h>

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
	char buf[4];
	gb_reg_profile(gb);
	show_regs(gb->reg,32);
	show_regs(gb->reg,16);
	show_regs(gb->reg,8);
	show_gb_flag_regs(gb->reg);
	r_asm_setup(gb->a,"gb",8,0);
	r_asm_set_pc(gb->a, r_reg_getv(gb->reg, "mpc"));
	r_io_read_at(gb->io, r_reg_getv(gb->reg, "mpc"),buf, 4);
	r_asm_disassemble(gb->a, gb->op, buf, 4);
	eprintf("0x%08x\t%s\t;\n",r_reg_getv(gb->reg, "mpc"), gb->op->buf_asm);
	gb->mbc->type = gb_get_mbc(gb->io);
	eprintf("MBC-type:\t%i\nLD-DEMO\n",gb->mbc->type);
	gb_ld_mov(gb->reg, "bc", "sp");
	show_regs(gb->reg, 16);
	gb_ldi_mov(gb->reg, "de", "sp");
	show_regs(gb->reg, 16);
	gb_ldd_mov(gb->reg, "hl", "sp");
	show_regs(gb->reg, 16);
	gb_dec(gb->reg, "b");
	show_regs(gb->reg, 16);
	gb_swap_reg(gb->reg, "b");
	show_regs(gb->reg, 16);
	r_io_close(gb->io,gb->io->fd);
	gb_emu_free(gb);
}