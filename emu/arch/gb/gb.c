/* ramulate - LGPL - Copyright 2013 - 2014 - condret@runas-racer.com */

#include <emu.h>
#include <gb.h>
#include <r_bin.h>
#include <r_asm.h>
#include <r_io.h>
#include <r_anal.h>

void gb_mbc_new(emu *e)
{
	GBmbc *mbc = R_NEW0(GBmbc);
	gb_get_mbc(e->io, mbc);
	e->data = mbc;
}

int gb_set_vs_profile(emu *e)										//we need a parser here in /emu/vsection.c
{
	virtual_section_add(e, 0x8000, 0x1fff, 7, "vram");
	virtual_section_add(e, 0xc000, 0x1fff, 7, "internal_ram");
	virtual_section_add(e, 0xe000, 0x1dff, 0, "FUCK YOU!");
	virtual_section_add(e, 0xfe00, 0xa0, 7, "OAM");
	virtual_section_add(e, 0xff00, 0x4c, 7, "IO-Ports");
	virtual_section_add(e, 0xff80, 0x7e, 7, "zram");
	virtual_section_add(e, 0xffff, 1, 7, "ie-flg");
	GBmbc *mbc;
	mbc = (GBmbc *) e->data;
	int i, j;
	switch(mbc->rambanks) {
		case GB_NO_RAMBANKS:
			i = 0;
			eprintf("no rambanks here\n");
			break;
		case GB_1_2K_RAMBANKS:
			virtual_section_add(e, 0xa000, 0x800, 7, "rambank00");
			i = 0;
			break;
		case GB_1_8K_RAMBANKS:
			virtual_section_add(e, 0xa000, 0x2000, 7, "rambank00");
			i = 0;
			break;
		default:
			switch(mbc->rambanks) {
				case GB_4_8K_RAMBANKS:
					i = 3;
					break;
				case GB_16_8K_RAMBANKS:
					i = 15;
					break;
			}
			break;
	}
	if(i) {
		char *name = malloc(VS_MAX_NAME_LEN+1);
		for(j=0;j<=i;j++) {
			snprintf(name, VS_MAX_NAME_LEN, "rambank%02x", j);
			virtual_section_add(e, 0xa000 + (0x10000*j), 0x2000, 7, name);
		}
		free(name);
	}
	ut8 wbuf = 0;
	emu_write(e, 0xff05, &wbuf, 1);
	emu_write(e, 0xff06, &wbuf, 1);
	emu_write(e, 0xff07, &wbuf, 1);
	emu_write(e, 0xff17, &wbuf, 1);
	emu_write(e, 0xff21, &wbuf, 1);
	emu_write(e, 0xff22, &wbuf, 1);
	emu_write(e, 0xff42, &wbuf, 1);
	emu_write(e, 0xff43, &wbuf, 1);
	emu_write(e, 0xff45, &wbuf, 1);
	emu_write(e, 0xff4a, &wbuf, 1);
	emu_write(e, 0xff4b, &wbuf, 1);
	emu_write(e, 0xffff, &wbuf, 1);
	wbuf = 0x80;
	emu_write(e, 0xff10, &wbuf, 1);
	wbuf = 0xbf;
	emu_write(e, 0xff11, &wbuf, 1);
	emu_write(e, 0xff14, &wbuf, 1);
	emu_write(e, 0xff19, &wbuf, 1);
	emu_write(e, 0xff1e, &wbuf, 1);
	wbuf = 0xf3;
	emu_write(e, 0xff12, &wbuf, 1);
	emu_write(e, 0xff25, &wbuf, 1);
	wbuf = 0x3f;
	emu_write(e, 0xff16, &wbuf, 1);
	wbuf = 0x7f;
	emu_write(e, 0xff1a, &wbuf, 1);
	wbuf = 0xff;
	emu_write(e, 0xff1b, &wbuf, 1);
	emu_write(e, 0xff20, &wbuf, 1);
	emu_write(e, 0xff48, &wbuf, 1);
	emu_write(e, 0xff49, &wbuf, 1);
	wbuf = 0x9f;
	emu_write(e, 0xff1c, &wbuf, 1);
	wbuf = 0x77;
	emu_write(e, 0xff24, &wbuf, 1);
	wbuf = 0xf3;
	emu_write(e, 0xff25, &wbuf, 1);
	wbuf = 0xf1;
	emu_write(e, 0xff26, &wbuf, 1);
	wbuf = 0x91;
	emu_write(e, 0xff40, &wbuf, 1);
	wbuf = 0xfc;
	emu_write(e, 0xff47, &wbuf, 1);
	wbuf = 0x91;
	emu_write(e, 0xff44, &wbuf, 1);
	return R_TRUE;
}

int gb_set_reg_profile(emu *e)
{
	int ret = r_anal_set_reg_profile (e->anal);
	e->reg = e->anal->reg;
	r_reg_set_value (e->reg, r_reg_get (e->reg,"mpc",-1), ((RBinAddr *) r_list_get_n (r_bin_get_entries (e->bin), 0))->offset);
	r_reg_set_value (e->reg, r_reg_get (e->reg,"sp",-1), 0xfffe);
	r_reg_set_value (e->reg, r_reg_get (e->reg,"af",-1), 0x01b0);
	r_reg_set_value (e->reg, r_reg_get (e->reg,"bc",-1), 0x0013);
	r_reg_set_value (e->reg, r_reg_get (e->reg,"de",-1), 0x00d8);
	r_reg_set_value (e->reg, r_reg_get (e->reg,"hl",-1), 0x014d);
	r_reg_set_value (e->reg, r_reg_get (e->reg,"ime",-1), R_TRUE);
	return ret;
}

void gb_get_mbc(RIO *io, GBmbc *mbc)
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
			mbc->type = GB_ROM;
			break;
		case 1:
		case 2:
		case 3:
		case 0xff:		//huc1
			mbc->type = GB_MBC1;
			break;
		case 5:
		case 6:
			mbc->type = GB_MBC2;
			break;
		case 0xf:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
			mbc->type = GB_MBC3;
			break;
		case 0x19:
		case 0x1a:
		case 0x1b:
		case 0x1c:
		case 0x1d:
		case 0x1e:
			mbc->type = GB_MBC5;
			break;
		case 0xfd:		//?
			mbc->type = GB_TAMA;
			break;
		case 0xfe:		//?
			mbc->type = GB_HUC3;
			break;
		default:
			mbc->type = GB_UMBC;
	}
	r_io_read_at(io, 0x149, &buf, 1);
	mbc->rambanks = buf;
}

int gb_step(emu* e, ut8 *buf)
{
	int ret = R_FALSE;
	r_reg_set_value(e->reg, r_reg_get(e->reg, "pc", -1), r_reg_getv(e->reg, "pc") + e->op->size);
	switch(buf[0]) {
		case 0x00:
			ret = R_TRUE;
			break;
		case 0x01:
		case 0x11:
		case 0x21:
		case 0x31:
			e->op->buf_asm[5] = 0;
			ret = gb_ld_store_const(e->reg, &e->op->buf_asm[3], (buf[2]*0x100)+buf[1]);
			break;
		case 0x06:
		case 0x0e:
		case 0x16:
		case 0x1e:
		case 0x26:
		case 0x2e:
		case 0x3e:
			e->op->buf_asm[4] = 0;
			ret = gb_ld_store_const(e->reg, &e->op->buf_asm[3], buf[1]);
			break;
		case 0x18:
			ret = gb_jmp_rel(e->reg, (st8)buf[1]);
			break;
		case 0x20:
		case 0x28:
		case 0x30:
		case 0x38:
			e->op->buf_asm[strlen(e->op->buf_asm)-6] = 0;
			ret = gb_jmp_rel_cond(e->reg, &e->op->buf_asm[3], (st8)buf[1]);
			break;
		case 0x22:													//ldi
			if(gb_ld_load_to(e, e->reg, r_reg_getv(e->reg, "hl"), "a") && gb_inc(e->reg, "hl"))
				ret = R_TRUE;
			break;
		case 0x1a:
			ret = gb_ld_store_from(e, e->reg, "a", r_reg_getv(e->reg, "de"));					//can we use r_anal here
			break;
		case 0x56:
			ret = gb_ld_store_from(e, e->reg, "d", r_reg_getv(e->reg, "hl"));
			break;
		case 0x5e:
			ret = gb_ld_store_from(e, e->reg, "e", r_reg_getv(e->reg, "hl"));
			break;
		case 0x66:
			ret = gb_ld_store_from(e, e->reg, "h", r_reg_getv(e->reg, "hl"));
			break;
		case 0x7e:
			ret = gb_ld_store_from(e, e->reg, "a", r_reg_getv(e->reg, "hl"));
			break;
		case 0x2a:													//ldi
			if(gb_ld_store_from(e, e->reg, "a", r_reg_getv(e->reg, "hl")) && gb_inc(e->reg, "hl"))
				ret = R_TRUE;
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
			ret = gb_inc(e->reg, &e->op->buf_asm[4]);
			break;
		case 0x34:
			ret = gb_inc_at(e, e->reg, r_reg_getv(e->reg, "hl"));
			break;
		case 0x32:													//ldd
			if(gb_ld_load_to(e, e->reg, r_reg_getv(e->reg, "hl"), "a") && gb_dec(e->reg, "hl"))
				ret = R_TRUE;
			break;
		case 0x36:
			ret = gb_ld_load_const_to(e, r_reg_getv(e->reg, "hl"), buf[1]);
			break;
		case 0x3a:													//ldd
			if(gb_ld_store_from(e, e->reg, "a", r_reg_getv(e->reg, "hl")) && gb_dec(e->reg, "hl"))
				ret = R_TRUE;
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
			ret = gb_dec(e->reg, &e->op->buf_asm[4]);
			break;
		case 0x2f:
			r_reg_set_value(e->reg, r_reg_get(e->reg, "H", -1), R_TRUE);
			ret = r_reg_set_value(e->reg, r_reg_get(e->reg, "N", -1), R_TRUE);
			break;
		case 0x37:
			ret = r_reg_set_value(e->reg, r_reg_get(e->reg, "C", -1), R_TRUE);					//scf
			break;
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
			e->op->buf_asm[4] = 0;
			ret = gb_ld_mov(e->reg, &e->op->buf_asm[3], &e->op->buf_asm[6]);
			break;
		case 0x12:
		case 0x70:
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
		case 0x75:
		case 0x77:
			e->op->buf_asm[6] = 0;
			ret = gb_ld_load_to(e, e->reg, r_reg_getv(e->reg, &e->op->buf_asm[4]), &e->op->buf_asm[9]);
			break;
		case 0x76:
			if(!r_reg_getv(e->reg, "ime")) {
				r_anal_op(e->anal, e->anop, r_reg_getv(e->reg, "mpc"), buf, 8);
				gb_jmp(e->reg, e->anop->jump);
			}
			ret = R_TRUE;
			break;
		case 0xc6:
			ret = gb_add_8_const(e->reg, buf[1]);
			break;
		case 0x80:
		case 0x81:
		case 0x82:
		case 0x83:
		case 0x84:
		case 0x85:
		case 0x87:
			ret = gb_add_8(e->reg, &e->op->buf_asm[4]);
			break;
		case 0x86:
			ret = gb_add_8_at(e, e->reg, r_reg_getv(e->reg, "hl"));
			break;
		case 0x09:
		case 0x19:
		case 0x29:
		case 0x39:
			ret = gb_add_16(e->reg, "hl", &e->op->buf_asm[8]);
			break;
		case 0x88:
		case 0x89:
		case 0x8a:
		case 0x8b:
		case 0x8c:
		case 0x8d:
		case 0x8f:
			ret = gb_adc(e->reg, &e->op->buf_asm[4]);
			break;
		case 0x90:
		case 0x91:
		case 0x92:
		case 0x93:
		case 0x94:
		case 0x95:
		case 0x96:
			ret = gb_sub(e->reg, &e->op->buf_asm[4]);
			break;
		case 0x98:
		case 0x99:
		case 0x9a:
		case 0x9b:
		case 0x9c:
		case 0x9d:
		case 0x9f:
			ret = gb_sbc(e->reg, &e->op->buf_asm[4]);
			break;
		case 0xa0:
		case 0xa1:
		case 0xa2:
		case 0xa3:
		case 0xa4:
		case 0xa5:
		case 0xa7:
			ret = gb_and(e->reg, &e->op->buf_asm[4]);
			break;
		case 0xa6:
			ret = gb_and_at(e, e->reg, r_reg_getv(e->reg, "hl"));
			break;
		case 0xe6:
			ret = gb_and_const(e->reg, buf[1]);
			break;
		case 0xa8:
		case 0xa9:
		case 0xaa:
		case 0xab:
		case 0xac:
		case 0xad:
		case 0xaf:
			ret = gb_xor(e->reg, &e->op->buf_asm[4]);
			break;
		case 0xae:
			ret = gb_xor_at(e, e->reg, r_reg_getv(e->reg, "hl"));
			break;
		case 0xb0:
		case 0xb1:
		case 0xb2:
		case 0xb3:
		case 0xb4:
		case 0xb5:
		case 0xb7:
			ret = gb_or(e->reg, &e->op->buf_asm[3]);
			break;
		case 0xb6:
			ret = gb_or_at(e, e->reg, r_reg_getv(e->reg, "hl"));
			break;
		case 0xb9:
			ret = gb_cp(e->reg, "c");
			break;
		case 0xbe:
			ret = gb_cp_at(e, e->reg, r_reg_getv(e->reg, "hl"));
			break;
		case 0xc3:
			ret = gb_jmp(e->reg, (buf[2]*0x100)+buf[1]);
			break;
		case 0xe9:
			ret = gb_jmp(e->reg, r_reg_getv(e->reg, "hl"));
			break;
		case 0xc2:
		case 0xca:
		case 0xd2:
		case 0xda:
			e->op->buf_asm[strlen(e->op->buf_asm)-8] = 0;
			ret = gb_jmp_cond(e->reg, &e->op->buf_asm[3], (buf[2]*0x100)+buf[1]);
			break;
		case 0xcd:
			ret = gb_call(e, e->reg, (buf[2]*0x100)+buf[1]);
			break;
		case 0xc4:
		case 0xcc:
		case 0xd4:
		case 0xdc:
			e->op->buf_asm[strlen(e->op->buf_asm)-8] = 0;
			ret = gb_call_cond(e, e->reg, &e->op->buf_asm[5], (buf[2]*0x100)+buf[1]);
			break;
		case 0xc9:
			ret = gb_ret(e, e->reg);
			break;
		case 0xd9:
			ret = gb_reti(e, e->reg);
			break;
		case 0xc0:
		case 0xc8:
		case 0xd0:
		case 0xd8:
			ret = gb_ret_cond(e, e->reg, &e->op->buf_asm[4]);
			break;
		case 0xc5:
		case 0xd5:
		case 0xe5:
		case 0xf5:
			ret = gb_push(e, e->reg, &e->op->buf_asm[5]);
			break;
		case 0xc1:
		case 0xd1:
		case 0xe1:
		case 0xf1:
			ret = gb_pop(e, e->reg, &e->op->buf_asm[4]);
			break;
		case 0xe0:
			ret = gb_ld_load_to(e, e->reg, buf[1]+0xff00, "a");
			break;
		case 0xe2:
			ret = gb_ld_load_to(e, e->reg, r_reg_getv(e->reg, "c") + 0xff00, "a");
			break;
		case 0xea:
			ret = gb_ld_load_to(e, e->reg, (buf[2]*0x100)+buf[1], "a");
			break;
		case 0xf0:
			ret = gb_ld_store_from(e, e->reg, "a", buf[1]+0xff00);
			break;
		case 0xfa:
			ret = gb_ld_store_from(e, e->reg, "a", (buf[2] * 0x100) + buf[1]);
			break;
		case 0xf3:
			ret = gb_di(e->reg);
			break;
		case 0xf9:
			ret = gb_ld_mov(e->reg, "sp", "hl");
			break;
		case 0xfb:
			ret = gb_ei(e->reg);
			break;
		case 0xfe:
			ret = gb_cp_const(e->reg, buf[1]);
			break;
		case 0xc7:
			ret = gb_call(e, e->reg, 0);
			break;
		case 0xcf:
			ret = gb_call(e, e->reg, 8);
			break;
		case 0xd7:
			ret = gb_call(e, e->reg, 16);
			break;
		case 0xdf:
			ret = gb_call(e, e->reg, 24);
			break;
		case 0xe7:
			ret = gb_call(e, e->reg, 32);
			break;
		case 0xef:
			ret = gb_call(e, e->reg, 40);
			break;
		case 0xf7:
			ret = gb_call(e, e->reg, 48);
			break;
		case 0xff:
			ret = gb_call(e, e->reg, 56);
			break;
		case 0xcb:
			switch(buf[1]/8) {
				case 0:
					if(buf[1] == 6)
						ret = R_FALSE;
					else 	ret = gb_rlc(e->reg, &e->op->buf_asm[4]);
					break;
				case 1:
					if(buf[1]%8 == 6)
						ret = R_FALSE;
					else	ret = gb_rrc(e->reg, &e->op->buf_asm[4]);
					break;
				case 2:
					if(buf[1]%8 == 6)
						ret = gb_rl_at(e, e->reg, r_reg_getv(e->reg, "hl"));
					else	ret = gb_rl(e->reg, &e->op->buf_asm[3]);
					break;
				case 3:
					if(buf[1]%8 == 6)
						ret = gb_rr_at(e, e->reg, r_reg_getv(e->reg, "hl"));
					else	ret = gb_rr(e->reg, &e->op->buf_asm[3]);
					break;
				case 4:
					if(buf[1]%8 == 6)
						ret = gb_sla_at(e, e->reg, r_reg_getv(e->reg, "hl"));
					else	ret = gb_sla(e->reg, &e->op->buf_asm[4]);
					break;
				case 5:
					if(buf[1]%8 == 6)
						ret = gb_sra_at(e, e->reg, r_reg_getv(e->reg, "hl"));
					else	ret = gb_sra(e->reg, &e->op->buf_asm[4]);
					break;
				case 6:
					if(buf[1]%8 == 6)
						ret = gb_swap_at(e, r_reg_getv(e->reg, "hl"));
					else	ret = gb_swap(e->reg, &e->op->buf_asm[5]);
					break;
				case 7:
					if(buf[1]%8 == 6)
						ret = gb_srl_at(e, e->reg, r_reg_getv(e->reg, "hl"));
					else	ret = gb_srl(e->reg, &e->op->buf_asm[4]);
					break;
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
					if(buf[1]%8 == 6)
						ret = gb_bit_at(e, e->reg, (buf[1]/8)-8, r_reg_getv(e->reg, "hl"));
					else	ret = gb_bit(e->reg, (buf[1]/8)-8, &e->op->buf_asm[7]);
					break;
				case 16:
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
					if(buf[1]%8 == 6)
						ret = gb_res_at(e, (buf[1]/8)-16, r_reg_getv(e->reg, "hl"));
					else	ret = gb_res(e->reg, (buf[1]/8)-16, &e->op->buf_asm[7]);
					break;
				case 24:
				case 25:
				case 26:
				case 27:
				case 28:
				case 29:
				case 30:
				case 31:
					if(buf[1]%8 == 6)
						ret = gb_set_at(e, (buf[1]/8)-24, r_reg_getv(e->reg, "hl"));
					else	ret = gb_set(e->reg, (buf[1]/8)-24, &e->op->buf_asm[7]);
			}
	}
	if(r_reg_getv(e->reg, "pc")>0x3fff)
		r_reg_set_value(e->reg, r_reg_get(e->reg, "m", -1), r_reg_getv(e->reg, "mbcrom"));
	else	r_reg_set_value(e->reg, r_reg_get(e->reg, "m", -1), 0);
	return ret;
}


int gb_write(emu *e, ut64 addr, ut8 *buf, ut32 len)
{
	if(0x2000 <= addr && addr <= 0x3fff) {
		if(buf[0] == 0x20 || buf[0] == 0x40 || buf[0] == 0x60)
			return r_reg_set_value(e->reg, r_reg_get(e->reg, "mbcrom", -1), 0);
		if(!buf[0])
			return r_reg_set_value(e->reg, r_reg_get(e->reg, "mbcrom", -1), 0);
		return r_reg_set_value(e->reg, r_reg_get(e->reg, "mbcrom", -1), buf[0]-1);
	}
	if(0x4000 <= addr && addr <= 0x5fff) {
		if(!buf[0])
			return r_reg_set_value(e->reg, r_reg_get(e->reg, "mbcram", -1), 0);
		return r_reg_set_value(e->reg, r_reg_get(e->reg, "mbcram", -1), buf[0]-1);
	}
	if(0xa000 <= addr && addr <= 0xc000)
		return emu_write(e, addr + (r_reg_getv(e->reg, "mbcram") << 16), buf, len);
	return emu_write(e, addr, buf, len);
}

int gb_read(emu *e, ut64 addr, ut8 *buf, ut32 len)
{
	if(0x4000 <= addr && addr <= 0x7fff)
		return emu_read(e, addr + (r_reg_getv(e->reg, "mbcrom") << 16), buf, len);
	if(0xa000 <= addr && addr <= 0xc000)
		return emu_read(e, addr + (r_reg_getv(e->reg, "mbcram") << 16), buf, len);
	return emu_read(e, addr, buf, len);
}
