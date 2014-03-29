/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <emu.h>
#include <gb.h>
#include <stdlib.h>
#include <r_asm.h>
#include <r_io.h>
#include <r_reg.h>

int gb_mov (emu *e)
{
	ut64 val = 0;
	if (e->anop->src[0]->reg)
		val = r_reg_get_value (e->reg, e->anop->src[0]->reg);
	else	val = e->anop->src[0]->imm;
	if (e->anop->src[1]) {
		if (e->anop->src[1]->reg) {
			if (e->anop->src[1]->absolute)
				val += r_reg_get_value (e->reg, e->anop->src[1]->reg);
			else	val += (st64)r_reg_get_value (e->reg, e->anop->src[1]->reg);
		} else {
			if (e->anop->src[1]->absolute)
				val += (ut64)e->anop->src[1]->imm;
			else	val += e->anop->src[1]->imm;
		}
	}
	return r_reg_set_value (e->reg, e->anop->dst->reg, val);
}

int gb_xor (emu *e)
{
	ut64 val = r_reg_get_value (e->reg, e->anop->dst->reg), src;
	ut8 buf[1];
	switch (e->op->buf_asm[2]) {
		case 'l':				//cpl
			r_reg_set_value (e->reg, r_reg_get (e->reg, "N", -1), R_TRUE);
			r_reg_set_value (e->reg, r_reg_get (e->reg, "H", -1), R_TRUE);
			return r_reg_set_value (e->reg, e->anop->dst->reg, (val ^ 0xff));
		case 'f':				//ccf
			r_reg_set_value (e->reg, r_reg_get (e->reg, "N", -1), R_FALSE);
			r_reg_set_value (e->reg, r_reg_get (e->reg, "H", -1), R_FALSE);
			return r_reg_set_value (e->reg, e->anop->dst->reg, (val ^ 1));
	}
	r_reg_set_value (e->reg, r_reg_get (e->reg, "H", -1), R_FALSE);
	r_reg_set_value (e->reg, r_reg_get (e->reg, "N", -1), R_FALSE);
	r_reg_set_value (e->reg, r_reg_get (e->reg, "C", -1), R_FALSE);
	if (e->anop->src[0]->reg) {
		src = r_reg_get_value (e->reg, e->anop->src[0]->reg);
		if (e->anop->src[0]->memref) {
			gb_read (e, src, buf, 1);
			src = (ut64)*buf;
		}
	} else src = e->anop->src[0]->imm;
	val ^= src;
	r_reg_set_value (e->reg, r_reg_get (e->reg, "Z", -1), (val == 0));
	return r_reg_set_value (e->reg, e->anop->dst->reg, val);
}

static inline int gb_add_8 (emu *e)
{
	ut64 val = r_reg_get_value (e->reg, e->anop->dst->reg), src, addr;
	ut8 buf[1];
	if (e->anop->dst->memref) {			//inc [hl]
		addr = val;
		gb_read (e, val, buf, 1);
		val = (ut64)*buf;
	}
	if (e->anop->src[0]->reg) {
		src = r_reg_get_value (e->reg, e->anop->src[0]->reg);
		if (e->anop->src[0]->memref) {
			gb_read (e, src, buf, 1);
			src = (ut64)*buf;
		}
	} else	src = e->anop->src[0]->imm;
	if (e->anop->src[1])
		src += r_reg_get_value (e->reg, e->anop->src[1]->reg);
	if (e->op->buf_asm[0] != 'i');
		r_reg_set_value (e->reg, r_reg_get (e->reg, "C", -1), (((val + src) & 0xff) < val));
	r_reg_set_value (e->reg, r_reg_get (e->reg, "H", -1), (((val + src) & 0xf) < val));
	r_reg_set_value (e->reg, r_reg_get (e->reg, "Z", -1), (((val * src) & 0xff) == 0));
	val += src;
	val &= 0xff;
	if (e->anop->dst->memref) {
		*buf = (ut8)val;
		return gb_write (e, addr, buf, 1);
	}
	return r_reg_set_value (e->reg, e->anop->dst->reg, val);
}

int gb_add (emu *e)
{
	r_reg_set_value (e->reg, r_reg_get (e->reg, "N", -1), R_FALSE);
	if (e->anop->dst->reg->size == 8 || e->anop->dst->memref)
		return gb_add_8 (e);
	eprintf ("TODO\n");
	return R_FALSE;
}

int gb_sub (emu *e)
{
	ut64 val = r_reg_get_value (e->reg, e->anop->dst->reg), src;
	ut8 buf[1];
	if (e->anop->dst->reg->size == 16)
		return r_reg_set_value (e->reg, e->anop->dst->reg, (val-1) & 0xffff);
	r_reg_set_value (e->reg, r_reg_get (e->reg, "N", -1), R_TRUE);
	if (e->anop->src[0]->reg) {
		src = r_reg_get_value (e->reg, e->anop->src[0]->reg);
		if (e->anop->src[0]->memref) {
			gb_read (e, src, buf, 1);
			src = (ut64)*buf;
		}
	} else	src = e->anop->src[0]->imm;
	if (e->anop->src[1])
		src += r_reg_get_value (e->reg, e->anop->src[1]->reg);		//sbc
	if (e->anop->dst->reg->size != 2) {
		if (e->op->buf_asm[0] != 'd')
			r_reg_set_value (e->reg, r_reg_get (e->reg, "C", -1), (val<src));
		r_reg_set_value (e->reg, r_reg_get (e->reg, "H", -1), ((val & 7)<(src & 7)));
		r_reg_set_value (e->reg, r_reg_get (e->reg, "Z", -1), (val == src));
	}
	val -= src ;
	val &= 0xff;
	return r_reg_set_value (e->reg, e->anop->dst->reg, val);
}

int gb_cp (emu *e)
{
	ut64 tmp = r_reg_get_value (e->reg, e->anop->dst->reg);
	gb_sub (e);
	return r_reg_set_value (e->reg, e->anop->dst->reg, tmp);
}

int gb_jmp (emu *e)
{
	ut32 dst;
	dst = (e->anop->jump & 0xffff);				//this is safer
	RRegItem *pc = r_reg_get (e->reg, "mpc", -1);
	if (0x3fff < dst && dst < 0x8000)
		dst += 0x10000 * r_reg_getv (e->reg, "mbcrom");
	return r_reg_set_value (e->reg, pc, dst);
}

int gb_cjmp (emu *e)
{
	ut8 c = r_reg_get_value (e->reg, e->anop->dst->reg);
	if (e->anop->cond == R_ANAL_COND_EQ) {
		if (c==e->anop->src[0]->imm)
			return gb_jmp (e);
		return R_TRUE;
	}
	if (c!=e->anop->src[0]->imm)
		return gb_jmp (e);
	return R_TRUE;
}

int gb_ujmp (emu *e)
{
	if (e->anop->dst)
		e->anop->jump = r_reg_get_value (e->reg, e->anop->dst->reg);
	return gb_jmp (e);
}

int gb_call (emu *e)
{
	ut16 pc = (ut16)r_reg_getv (e->reg, "pc");
	ut16 sp = (ut16)r_reg_getv (e->reg, "sp");
	sp -= 2;
	r_reg_setv (e->reg, "sp", (ut64)sp);
	gb_write (e, (ut64)sp, (ut8*)(&pc), 2);
	return gb_jmp (e);
}

int gb_ccall (emu *e)
{
	ut8 c = r_reg_get_value (e->reg, e->anop->dst->reg);
	if (e->anop->cond == R_ANAL_COND_EQ) {
		if (c==e->anop->src[0]->imm)
			return gb_call (e);
		return R_TRUE;
	}
	if (c!=e->anop->src[0]->imm)
		return gb_call (e);
	return R_TRUE;
}

int gb_ret (emu *e)
{
	ut16 sp = (ut16)r_reg_getv (e->reg, "sp");
	ut16 pc = 0;
	ut32 mpc = 0;
	gb_read (e, (ut64)sp, (ut8*)(&pc), 2);
	mpc = pc;
	if (0x3fff < pc && pc < 0x8000)
		mpc += 0x10000 * r_reg_getv (e->reg, "mbcrom");
	return r_reg_setv (e->reg, "mpc", (ut64)mpc);
}

int gb_cret (emu *e)
{
	ut8 c = r_reg_get_value (e->reg, e->anop->dst->reg);
	if (e->anop->cond == R_ANAL_COND_EQ) {
		if (c==e->anop->src[0]->imm)
			return gb_ret (e);
		return R_TRUE;
	}
	if (c!=e->anop->src[0]->imm)
		return gb_ret (e);
	return R_TRUE;
}

int gb_store (emu *e)
{
	ut64 addr;
	ut16 buf;
	if (e->anop->src[0]->reg)
		buf = r_reg_get_value (e->reg, e->anop->src[0]->reg);
	else	buf = e->anop->src[0]->imm;
	if (e->anop->dst->reg)
		addr = r_reg_get_value (e->reg, e->anop->dst->reg);
	else {
		addr = e->anop->dst->base;
		if (e->anop->dst->regdelta)
			addr += r_reg_get_value (e->reg, e->anop->dst->regdelta);
	}
	buf = buf<<(8*(e->anop->dst->memref & 1));				//is this working good?
	if (e->op->buf_asm[2] == 'i') {						//ldi [hl]
		addr = (addr+1) & 0xffff;
		r_reg_set_value (e->reg, e->anop->dst->reg, addr);
	}
	if (e->op->buf_asm[2] == 'd') {
		addr = (addr-1) & 0xffff;
		r_reg_set_value (e->reg, e->anop->dst->reg, addr);
	}
	return gb_write (e, addr, (ut8*)(&buf), e->anop->dst->memref);
}

int gb_load (emu *e)
{
	ut64 addr;
	ut8 buf;
	if (e->anop->src[0]->reg)
		addr = r_reg_get_value (e->reg, e->anop->src[0]->reg);
	else {
		addr = e->anop->src[0]->base;
		if (e->anop->src[0]->regdelta)
			addr += r_reg_get_value (e->reg, e->anop->src[0]->regdelta);
	}
	gb_read (e, addr, &buf, 1);
	return r_reg_set_value (e->reg, e->anop->dst->reg, buf);
}
