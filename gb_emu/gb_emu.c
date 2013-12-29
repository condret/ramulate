/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <gb_emu.h>

GBemu *gb_emu_new() {
	struct gb_emu_t *gb = R_NEW0(GBemu);
	gb->reg = r_reg_new();
	gb->io = r_io_new();
	gb->bin = r_bin_new();
	gb->a = r_asm_new();
	gb->op = R_NEW0(RAsmOp);
	gb->mbc = gb_mbc_new();
	return gb;
}

void gb_emu_free(GBemu *gb) {
	r_reg_free(gb->reg);
	r_io_free(gb->io);
	r_bin_free(gb->bin);
	r_asm_free(gb->a);
	r_asm_op_free(gb->op);
	gb_mbc_free(gb->mbc);
	free(gb);
}

GBmbc *gb_mbc_new() {
	GBmbc *mbc = R_NEW0(GBmbc);
	return mbc;
}

int gb_reg_profile(GBemu *gb) {
	int ret = r_reg_set_profile_string (gb->reg,			//use ranal here
		"=pc	mpc\n"
		"=sp	sp\n"
		"=a0	af\n"
		"=a1	bc\n"
		"=a2	de\n"
		"=a3	hl\n"

		"gpr	mpc	.32	0	0\n"
		"gpr	pc	.16	0	0\n"
		"gpr	m	.16	2	0\n"

		"gpr	sp	.16	4	0\n"

		"gpr	af	.16	6	0\n"
		"gpr	f	.8	6	0\n"
		"gpr	a	.8	7	0\n"
		"gpr	Z	.1	.55	0\n"
		"gpr	N	.1	.54	0\n"
		"gpr	H	.1	.53	0\n"
		"gpr	C	.1	.52	0\n"

		"gpr	bc	.16	8	0\n"
		"gpr	c	.8	8	0\n"
		"gpr	b	.8	9	0\n"

		"gpr	de	.16	10	0\n"
		"gpr	e	.8	10	0\n"
		"gpr	d	.8	11	0\n"

		"gpr	hl	.16	12	0\n"
		"gpr	l	.8	12	0\n"
		"gpr	h	.8	13	0\n"

		"gpr	mbc	.16	14	0\n");
	r_reg_set_value(gb->reg,r_reg_get(gb->reg,"mpc",-1),((RBinAddr *)r_list_get_n(r_bin_get_entries(gb->bin), 0))->offset);
	r_reg_set_value(gb->reg,r_reg_get(gb->reg,"sp",-1),0xfffe);
	return ret;
}

st8 gb_get_mbc(RIO *io) {
	ut8 buf;
	r_io_read_at(io,0x147,&buf,1);
	switch(buf) {
		case 0:
		case 8:
		case 9:
		case 0xb:		//?
		case 0xc:		//?
		case 0xd:		//?
		case 0x1f:
			return GB_ROM;
		case 1:
		case 2:
		case 3:
		case 0xff:		//huc1
			return GB_MBC1;
		case 5:
		case 6:
			return GB_MBC2;
		case 0xf:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
			return GB_MBC3;
		case 0x19:
		case 0x1a:
		case 0x1b:
		case 0x1c:
		case 0x1d:
		case 0x1e:
			return GB_MBC5;
		case 0xfd:		//?
			return GB_TAMA;
		case 0xfe:		//?
			return GB_HUC3;
	}
	return GB_UMBC;
}

int gb_step(GBemu* gb){
	if(!gb)
		return R_FALSE;
	ut8 buf[4];
	r_io_read_at(gb->io, r_reg_getv(gb->reg, "mpc"), buf, 4);
	r_asm_set_pc(gb->a, r_reg_getv(gb->reg, "pc"));		//mpc does not really exist
	r_asm_disassemble(gb->a, gb->op, buf, 4);		//used for arg-parsing and op-size
	r_reg_set_value(gb->reg, r_reg_get(gb->reg, "pc", -1), r_reg_getv(gb->reg, "pc") + gb->op->size);
	switch(buf[0]) {
		case 0x00:
			return R_TRUE;				//TODO: fat-switch + arg-parsing
		case 0x18:
			return gb_jmp_rel(gb->reg, (st8)buf[1]);
		case 0xcd:
			return gb_call_jmp(gb->reg, (buf[2]*0x100)+buf[1]);
	}
	return R_TRUE;
}
