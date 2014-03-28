/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <emu.h>
#include <gb.h>
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
	ut64 val = r_reg_get_value (e->reg, e->anop->dst->reg), src;
	ut8 buf[1];
	if (e->anop->src[0]->reg) {
		src = r_reg_get_value (e->reg, e->anop->src[0]->reg);
		if (e->anop->src[0]->memref) {
			gb_read (e, src, buf, 1);
			src = (ut64)*buf;
		}
	} else	src = e->anop->src[0]->imm;
	if (e->anop->src[1])
		src += r_reg_get_value (e->reg, e->anop->src[1]->reg);
	r_reg_set_value (e->reg, r_reg_get (e->reg, "C", -1), (((val + src) & 0xff) < val));
	r_reg_set_value (e->reg, r_reg_get (e->reg, "H", -1), (((val + src) & 0xf) < val));
	r_reg_set_value (e->reg, r_reg_get (e->reg, "Z", -1), (((val * src) & 0xff) == 0));
	val += src;
	val &= 0xff;
	return r_reg_set_value (e->reg, e->anop->dst->reg, val);
}

int gb_add (emu *e)
{
	ut64 val = r_reg_get_value (e->reg, e->anop->dst->reg);
	if (e->op->buf_asm[0] == 'i')
		r_reg_set_value (e->reg, e->anop->dst->reg, (val+1) &0xffff);	//XXX memrefs
	r_reg_set_value (e->reg, r_reg_get (e->reg, "N", -1), R_FALSE);
	if (e->anop->dst->reg->size == 8)
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
	if (e->op->buf_asm[0] != 'd') {
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
