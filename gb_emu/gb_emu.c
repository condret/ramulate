/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <gb_emu.h>


GBemu *gb_emu_new()
{
	struct gb_emu_t *gb = R_NEW0(GBemu);
	gb->reg = r_reg_new();
	gb->io = r_io_new();
	gb->bin = r_bin_new();
	gb->a = r_asm_new();
	gb->op = R_NEW0(RAsmOp);
	gb->mbc = gb_mbc_new();
	return gb;
}

void gb_emu_free(GBemu *gb)
{
	r_reg_free(gb->reg);
	r_io_free(gb->io);
	r_bin_free(gb->bin);
	r_asm_free(gb->a);
	r_asm_op_free(gb->op);
	gb_mbc_free(gb->mbc);
	free(gb);
}

GBmbc *gb_mbc_new()
{
	GBmbc *mbc = R_NEW0(GBmbc);
	return mbc;
}

void gb_sections(RIO *io, RBin *bin)					//rombanks
{
	RList *sections = r_bin_get_sections(bin);
	RBinSection *section;
	RListIter *iter;
	r_list_foreach(sections, iter, section) {
		r_io_section_add(io,	section->offset, section->rva, section->size,
					section->vsize, section->srwx, section->name);
	}
}

int gb_reg_profile(GBemu *gb)
{
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

st8 gb_get_mbc(RIO *io)
{
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

int gb_step(GBemu* gb)
{
	if(!gb)
		return R_FALSE;
	int i;
	ut8 buf[4];
	if(r_reg_getv(gb->reg, "pc")<0x8000) {
		r_io_read_at(gb->io, r_reg_getv(gb->reg, "mpc"), buf, 4);
	} else {
		r_io_cache_read(gb->io, r_reg_getv(gb->reg, "pc"), buf, 4);	//stack-execution, ...
	}
	r_asm_set_pc(gb->a, r_reg_getv(gb->reg, "pc"));				//mpc does not really exist
	r_asm_disassemble(gb->a, gb->op, buf, 4);				//used for arg-parsing and op-size
	eprintf("<0x%08x>:\t", r_reg_getv(gb->reg, "mpc"));
	for(i=0; i< gb->op->size; i++)
		eprintf("%02x",buf[i]);
	eprintf("\t%s\n",gb->op->buf_asm);
	r_reg_set_value(gb->reg, r_reg_get(gb->reg, "pc", -1), r_reg_getv(gb->reg, "pc") + gb->op->size);
	switch(buf[0]) {
		case 0x00:							//nop
			return R_TRUE;						//TODO: more ops
		case 0x01:
		case 0x11:
		case 0x21:
		case 0x31:
			gb->op->buf_asm[5] = 0;
			return gb_ld_store_const(gb->reg, &gb->op->buf_asm[3], (buf[2]*0x100)+buf[1]);
		case 0x06:
		case 0x0e:
		case 0x16:
		case 0x1e:
		case 0x26:
		case 0x2e:
		case 0x3e:
			gb->op->buf_asm[4] = 0;
			return gb_ld_store_const(gb->reg, &gb->op->buf_asm[3], buf[1]);
		case 0x18:
			return gb_jmp_rel(gb->reg, (st8)buf[1]);
		case 0x20:
		case 0x28:
		case 0x30:
		case 0x38:
			gb->op->buf_asm[strlen(gb->op->buf_asm)-6] = 0;
			return gb_jmp_rel_cond(gb->reg, &gb->op->buf_asm[3], (st8)buf[1]);
		case 0x03:
		case 0x04:
		case 0x0c:
		case 0x13:
		case 0x14:
		case 0x1c:
		case 0x23:
		case 0x24:
		case 0x2c:
		case 0x33:
		case 0x3c:
			return gb_inc(gb->reg, &gb->op->buf_asm[4]);
		case 0x05:
		case 0x0b:
		case 0x0d:
		case 0x15:
		case 0x1b:
		case 0x1d:
		case 0x25:
		case 0x2b:
		case 0x2d:
		case 0x3b:
		case 0x3d:
			return gb_dec(gb->reg, &gb->op->buf_asm[4]);
		case 0x40:
		case 0x41:
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x47:
		case 0x48:
		case 0x49:
		case 0x4a:
		case 0x4b:
		case 0x4c:
		case 0x4d:
		case 0x4f:
		case 0x50:
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
		case 0x55:
		case 0x57:
		case 0x58:
		case 0x59:
		case 0x5a:
		case 0x5b:
		case 0x5c:
		case 0x5d:
		case 0x5f:
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
		case 0x64:
		case 0x65:
		case 0x67:
		case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
		case 0x6c:
		case 0x6d:
		case 0x6f:
		case 0x78:
		case 0x79:
		case 0x7a:
		case 0x7b:
		case 0x7c:
		case 0x7d:
		case 0x7f:
			gb->op->buf_asm[4] = 0;
			return gb_ld_mov(gb->reg, &gb->op->buf_asm[3], &gb->op->buf_asm[6]);
		case 0x88:
		case 0x89:
		case 0x8a:
		case 0x8b:
		case 0x8c:
		case 0x8d:
		case 0x8f:
			return gb_adc_reg(gb->reg, &gb->op->buf_asm[4]);
		case 0xa8:
		case 0xa9:
		case 0xaa:
		case 0xab:
		case 0xac:
		case 0xad:
		case 0xaf:
			return gb_xor_reg(gb->reg, &gb->op->buf_asm[4]);
		case 0xc3:
			return gb_jmp(gb->reg, (buf[2]*0x100)+buf[1]);
		case 0xe9:
			return gb_jmp(gb->reg, r_reg_getv(gb->reg, "hl"));
		case 0xc2:
		case 0xca:
		case 0xd2:
		case 0xda:
			gb->op->buf_asm[strlen(gb->op->buf_asm)-8] = 0;
			return gb_jmp_cond(gb->reg, &gb->op->buf_asm[3], (buf[2]*0x100)+buf[1]);
		case 0xcd:
			return gb_call(gb->io, gb->reg, (buf[2]*0x100)+buf[1]);
		case 0xc4:
		case 0xcc:
		case 0xd4:
		case 0xdc:
			gb->op->buf_asm[strlen(gb->op->buf_asm)-8] = 0;
			return gb_call_cond(gb->io, gb->reg, &gb->op->buf_asm[5], (buf[2]*0x100)+buf[1]);
		case 0xc9:
			return gb_ret(gb->io, gb->reg);
		case 0xd9:
			return gb_reti(gb->io, gb->reg);
		case 0xc0:
		case 0xc8:
		case 0xd0:
		case 0xd8:
			return gb_ret_cond(gb->io, gb->reg, &gb->op->buf_asm[4]);
		case 0xc5:
		case 0xd5:
		case 0xe5:
		case 0xf5:
			return gb_push(gb->io, gb->reg, &gb->op->buf_asm[5]);
		case 0xc1:
		case 0xd1:
		case 0xe1:
		case 0xf1:
			return gb_pop(gb->io, gb->reg, &gb->op->buf_asm[4]);
		case 0xe0:
			return gb_ld_load_to(gb->io, gb->reg, buf[1]+0xff00, "a");
		case 0xea:
			return gb_ld_load_to(gb->io, gb->reg, (buf[2]*0x100)+buf[1], "a");
		case 0xf0:
			return gb_ld_store_from(gb->io, gb->reg, "a", buf[1]+0xff00);
		case 0xf3:
			return gb_di(gb->io);
		case 0xfb:
			return gb_ei(gb->io);
		case 0xfe:
			return gb_cp_const(gb->reg, buf[1]);
		case 0xcb:
			switch(buf[1]/8) {
				case 16:
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
					if((buf[1]%8) == 6)
						return R_FALSE;
					return gb_res(gb->reg, (buf[1]/8)-16, &gb->op->buf_asm[7]);
			}
	}
	return R_FALSE;
}
