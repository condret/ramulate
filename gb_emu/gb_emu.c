/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <gb_emu.h>
#include <gb_ops.h>

GBemu *gb_emu_new()
{
	struct gb_emu_t *gb = R_NEW0(GBemu);
	gb->reg = r_reg_new();
	gb->io = r_io_new();
	gb->c = r_cache_new();
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
	r_cache_free(gb->c);
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

		"gpr	mbc	.16	14	0\n"

		"gpr	ime	.1	16	0\n");
	r_reg_set_value(gb->reg,r_reg_get(gb->reg,"mpc",-1),((RBinAddr *)r_list_get_n(r_bin_get_entries(gb->bin), 0))->offset);
	r_reg_set_value(gb->reg,r_reg_get(gb->reg,"sp",-1),0xfffe);
	r_reg_set_value(gb->reg,r_reg_get(gb->reg,"af",-1),0x01b0);
	r_reg_set_value(gb->reg,r_reg_get(gb->reg,"bc",-1),0x0013);
	r_reg_set_value(gb->reg,r_reg_get(gb->reg,"de",-1),0x00d8);
	r_reg_set_value(gb->reg,r_reg_get(gb->reg,"hl",-1),0x014d);
	r_reg_set_value(gb->reg,r_reg_get(gb->reg,"ime",-1),R_TRUE);
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

int emu_read(GBemu *gb, ut64 addr, ut8 *buf, int len)
{
	if(!(gb && buf))
		return R_FALSE;
	if(gb->c->base > addr + len || gb->c->base + gb->c->len < addr)
		return r_io_read_at(gb->io, addr, buf, len);
	if(gb->c->base + gb->c->len < addr + len) {
		memcpy(buf, gb->c->buf + (addr - gb->c->base), (gb->c->base + gb->c->len) - addr);
		return r_io_read_at(gb->io, gb->c->base + gb->c->len + 1, buf + ((gb->c->base + gb->c->len) - addr), len - ((gb->c->base + gb->c->len) - addr));
	}
	if(gb->c->base > addr) {
		r_io_read_at(gb->io, addr, buf, gb->c->base - addr);
		memcpy(buf + (addr - gb->c->base), gb->c->buf, len - (gb->c->base - addr));
		return R_TRUE;
	}
	memcpy(buf, gb->c->buf + addr - gb->c->base, len);
	return R_TRUE;
}

int emu_write(GBemu *gb, ut64 addr, ut8 *buf, int len)
{
	if(!(gb && buf))
		return R_FALSE;
	if((gb->c->base <= addr) && (gb->c->base + gb->c->len >= addr + len)) {
		memcpy(gb->c->buf + addr - gb->c->base, buf, len);
		return R_TRUE;
	}
	return R_FALSE;								//TODO
}

int gb_step(GBemu* gb)
{
	if(!gb)
		return R_FALSE;
	int i;
	ut8 buf[4];
	if(r_reg_getv(gb->reg, "pc")<0x8000) {
		emu_read(gb, r_reg_getv(gb->reg, "mpc"), buf, 4);
	} else {
		emu_read(gb, r_reg_getv(gb->reg, "pc"), buf, 4);		//stack-execution, ...
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
		case 0x22:													//ldi
			if(gb_ld_load_to(gb, gb->reg, r_reg_getv(gb->reg, "hl"), "a") && gb_inc(gb->reg, "hl"))
				return R_TRUE;
			break;
		case 0x2a:													//ldi
			if(gb_ld_store_from(gb, gb->reg, "a", r_reg_getv(gb->reg, "hl")) && gb_inc(gb->reg, "hl"))
				return R_TRUE;
			break;
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
		case 0x32:													//ldd
			if(gb_ld_load_to(gb, gb->reg, r_reg_getv(gb->reg, "hl"), "a") && gb_dec(gb->reg, "hl"))
				return R_TRUE;
			break;
		case 0x3a:													//ldd
			if(gb_ld_store_from(gb, gb->reg, "a", r_reg_getv(gb->reg, "hl")) && gb_dec(gb->reg, "hl"))
				return R_TRUE;
			break;
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
		case 0x37:
			return r_reg_set_value(gb->reg, r_reg_get(gb->reg, "C", -1), R_TRUE);					//scf
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
		case 0x80:
		case 0x81:
		case 0x82:
		case 0x83:
		case 0x84:
		case 0x85:
		case 0x87:
			return gb_add_8(gb->reg, &gb->op->buf_asm[4]);
		case 0x86:
			return gb_add_8_at(gb, gb->reg, r_reg_getv(gb->reg, "hl"));
		case 0x88:
		case 0x89:
		case 0x8a:
		case 0x8b:
		case 0x8c:
		case 0x8d:
		case 0x8f:
			return gb_adc(gb->reg, &gb->op->buf_asm[4]);
		case 0x90:
		case 0x91:
		case 0x92:
		case 0x93:
		case 0x94:
		case 0x95:
		case 0x96:
			return gb_sub(gb->reg, &gb->op->buf_asm[4]);
		case 0x98:
		case 0x99:
		case 0x9a:
		case 0x9b:
		case 0x9c:
		case 0x9d:
		case 0x9f:
			return gb_sbc(gb->reg, &gb->op->buf_asm[4]);
		case 0xa0:
		case 0xa1:
		case 0xa2:
		case 0xa3:
		case 0xa4:
		case 0xa5:
		case 0xa7:
			return gb_and(gb->reg, &gb->op->buf_asm[4]);
		case 0xa6:
			return gb_and_at(gb, gb->reg, r_reg_getv(gb->reg, "hl"));
		case 0xa8:
		case 0xa9:
		case 0xaa:
		case 0xab:
		case 0xac:
		case 0xad:
		case 0xaf:
			return gb_xor(gb->reg, &gb->op->buf_asm[4]);
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
			return gb_call(gb, gb->reg, (buf[2]*0x100)+buf[1]);
		case 0xc4:
		case 0xcc:
		case 0xd4:
		case 0xdc:
			gb->op->buf_asm[strlen(gb->op->buf_asm)-8] = 0;
			return gb_call_cond(gb, gb->reg, &gb->op->buf_asm[5], (buf[2]*0x100)+buf[1]);
		case 0xc9:
			return gb_ret(gb, gb->reg);
		case 0xd9:
			return gb_reti(gb, gb->reg);
		case 0xc0:
		case 0xc8:
		case 0xd0:
		case 0xd8:
			return gb_ret_cond(gb, gb->reg, &gb->op->buf_asm[4]);
		case 0xc5:
		case 0xd5:
		case 0xe5:
		case 0xf5:
			return gb_push(gb, gb->reg, &gb->op->buf_asm[5]);
		case 0xc1:
		case 0xd1:
		case 0xe1:
		case 0xf1:
			return gb_pop(gb, gb->reg, &gb->op->buf_asm[4]);
		case 0xe0:
			return gb_ld_load_to(gb, gb->reg, buf[1]+0xff00, "a");
		case 0xea:
			return gb_ld_load_to(gb, gb->reg, (buf[2]*0x100)+buf[1], "a");
		case 0xf0:
			if(buf[1] == 0x44)
				return gb_ld_store_from(gb, gb->reg, "a", 0xff44);
			return gb_ld_store_from(gb, gb->reg, "a", buf[1]+0xff00);
		case 0xf3:
			return gb_di(gb->reg);
		case 0xfb:
			return gb_ei(gb->reg);
		case 0xfe:
			return gb_cp_const(gb->reg, buf[1]);
		case 0xc7:
			return gb_call(gb, gb->reg, 0);
		case 0xcf:
			return gb_call(gb, gb->reg, 8);
		case 0xd7:
			return gb_call(gb, gb->reg, 16);
		case 0xdf:
			return gb_call(gb, gb->reg, 24);
		case 0xe7:
			return gb_call(gb, gb->reg, 32);
		case 0xef:
			return gb_call(gb, gb->reg, 40);
		case 0xf7:
			return gb_call(gb, gb->reg, 48);
		case 0xff:
			return gb_call(gb, gb->reg, 56);
		case 0xcb:
			switch(buf[1]/8) {
				case 2:
					if(buf[1]%8 == 6)
						return gb_rl_at(gb, gb->reg, r_reg_getv(gb->reg, "hl"));
					return gb_rl(gb->reg, &gb->op->buf_asm[3]);
				case 3:
					if(buf[1]%8 == 6)
						return gb_rr_at(gb, gb->reg, r_reg_getv(gb->reg, "hl"));
					return gb_rr(gb->reg, &gb->op->buf_asm[3]);
				case 4:
					if(buf[1]%8 == 6)
						return gb_sla_at(gb, gb->reg, r_reg_getv(gb->reg, "hl"));
					return gb_sla(gb->reg, &gb->op->buf_asm[4]);
				case 5:
					if(buf[1]%8 == 6)
						return gb_sra_at(gb, gb->reg, r_reg_getv(gb->reg, "hl"));
					return gb_sra(gb->reg, &gb->op->buf_asm[4]);
				case 6:
					if(buf[1]%8 == 6)
						return gb_swap_at(gb, r_reg_getv(gb->reg, "hl"));
					return gb_swap(gb->reg, &gb->op->buf_asm[5]);
				case 7:
					if(buf[1]%8 == 6)
						return gb_srl_at(gb, gb->reg, r_reg_getv(gb->reg, "hl"));
					return gb_srl(gb->reg, &gb->op->buf_asm[4]);
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
					if(buf[1]%8 == 6)
						return gb_bit_at(gb, gb->reg, (buf[1]/8)-8, r_reg_getv(gb->reg, "hl"));
					return gb_bit(gb->reg, (buf[1]/8)-8, &gb->op->buf_asm[7]);
				case 16:
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
					if(buf[1]%8 == 6)
						return gb_res_at(gb, (buf[1]/8)-16, r_reg_getv(gb->reg, "hl"));
					return gb_res(gb->reg, (buf[1]/8)-16, &gb->op->buf_asm[7]);
				case 24:
				case 25:
				case 26:
				case 27:
				case 28:
				case 29:
				case 30:
				case 31:
					if(buf[1]%8 == 6)
						return gb_set_at(gb, (buf[1]/8)-24, r_reg_getv(gb->reg, "hl"));
					return gb_set(gb->reg, (buf[1]/8)-24, &gb->op->buf_asm[7]);
			}
	}
	return R_FALSE;
}
