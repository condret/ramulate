/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <gb_ops.h>
#include <r_asm.h>
#include <r_io.h>		//we need rio-cache here
#include <r_reg.h>

int gb_push(RIO *io, RReg *reg, const char *src)					//might be weak
{
	if(!(io && reg && src))
		return R_FALSE;
//	if(!(r_reg_getv(reg, "sp")>0x8001))						//check this please
//		return R_FALSE;
	ut16 *regval;
	ut8 push[2];
	regval = (ut16 *)push;
	*regval = r_reg_getv(reg, src);
	r_io_cache_write(io, r_reg_getv(reg, "sp") -2, push, 2);
	return r_reg_set_value(reg, r_reg_get(reg, "sp", -1), r_reg_getv(reg, "sp") -2);
}

int gb_pop(RIO *io, RReg *reg, const char *dest)					//might be weak
{
	if(!(io && reg && dest))
		return R_FALSE;
//	if(!(r_reg_getv(reg, "sp")<0xfffc))						//check this please
//		return R_FALSE;
	ut16 *regval;
	ut8 pop[2];
	regval = (ut16 *)pop;
	r_io_cache_read(io, r_reg_getv(reg, "sp"), pop, 2);
	r_reg_set_value(reg, r_reg_get(reg, dest, -1), *regval);
	return r_reg_set_value(reg, r_reg_get(reg, "sp", -1), r_reg_getv(reg, "sp") +2);
}

int gb_ei(RIO *io)
{
	if(!io)
		return R_FALSE;
	ut8 buf = 1;
	r_io_cache_write(io, 0xffff, &buf, 1);						//todo: enum for hw-regs
	return R_TRUE;
}

int gb_di(RIO *io)
{
	if(!io)
		return R_FALSE;
	ut8 buf = 0;
	r_io_cache_write(io, 0xffff, &buf, 1);						//todo: enum for hw-regs
	return R_TRUE;
}

int gb_ld_mov(RReg *reg, const char *dest, const char *src)
{
	if(!(reg && dest && src))
		return R_FALSE;
	return r_reg_set_value(reg, r_reg_get(reg, dest, -1), r_reg_getv(reg, src));
}

int gb_ldi_mov(RReg *reg, const char *dest, const char *src)				//ld+inc
{
	if(!(reg && dest && src))
		return R_FALSE;
	return r_reg_set_value(reg, r_reg_get(reg, dest, -1), r_reg_getv(reg, src)+1);
}

int gb_ldd_mov(RReg *reg, const char *dest, const char *src)				//ld+dec
{
	if(!(reg && dest && src))
		return R_FALSE;
	return r_reg_set_value(reg, r_reg_get(reg, dest, -1), r_reg_getv(reg, src)-1);
}

int gb_ld_store_const(RReg *reg, const char *dest, const ut16 src)
{
	if(!(reg && dest))
		return R_FALSE;
	return r_reg_set_value(reg, r_reg_get(reg, dest, -1), src);
}

int gb_ld_load_to(RIO *io, RReg *reg, const ut16 dest, const char *src)
{
	if(!(io && reg && src))
		return R_FALSE;
	if(dest < 0x8000)
		return R_TRUE;
	ut8 buf = r_reg_getv(reg, src);
	r_io_cache_write(io, dest, &buf, 1);
	return R_TRUE;
}

int gb_ld_store_from(RIO *io, RReg *reg, const char *dest, const ut16 src)
{
	if(!io)
		return R_FALSE;
	ut8 buf;
	r_io_cache_read(io, src, &buf, 1);
	return gb_ld_store_const(reg, dest, buf);
}

int gb_inc(RReg *reg, const char *dest)							//inc
{
	if(!(reg && dest))
		return R_FALSE;
	return r_reg_set_value(reg, r_reg_get(reg, dest, -1), r_reg_getv(reg, dest)+1);
}

int gb_dec(RReg *reg, const char *dest)							//dec
{
	if(!(reg && dest))
		return R_FALSE;
	return r_reg_set_value(reg, r_reg_get(reg, dest, -1), r_reg_getv(reg, dest)-1);
}

int gb_add_8(RReg *reg, const char *dest, const char *src)
{
	if(!(reg && dest && src))
		return R_FALSE;
	int ret = r_reg_set_value(reg, r_reg_get(reg, dest, -1), (ut8)(r_reg_getv(reg, dest) + r_reg_getv(reg, src)));
	if(r_reg_getv(reg, dest))
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_FALSE);
	else
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_TRUE);
	return ret;
}

int gb_adc_reg(RReg *reg, const char *src)
{
	if(!(reg && src))
		return R_FALSE;
	if((r_reg_getv(reg, "a") % 16) > ((r_reg_getv(reg, "a") + r_reg_getv(reg, src) + r_reg_getv(reg, "C"))%16))
		r_reg_set_value(reg, r_reg_get(reg, "H", -1), R_TRUE);
	else
		r_reg_set_value(reg, r_reg_get(reg, "H", -1), R_FALSE);
	r_reg_set_value(reg, r_reg_get(reg, "C", -1), (r_reg_getv(reg, "a") + r_reg_getv(reg, src) + r_reg_getv(reg, "C"))/256);
	int ret = r_reg_set_value(reg, r_reg_get(reg, "a", -1), (ut8)(r_reg_getv(reg, "a") + r_reg_getv(reg, src) + r_reg_getv(reg, "C")));
	if(r_reg_getv(reg, "a"))
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_FALSE);
	else
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_TRUE);
	r_reg_set_value(reg, r_reg_get(reg, "N", -1), R_FALSE);
	return ret;
}

int gb_sub_const(RReg *reg, const ut8 src)
{
	if(!reg)
		return R_FALSE;
	ut8 dval = r_reg_getv(reg, "a") - src;
	if(dval)
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_FALSE);
	else
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_TRUE);
	r_reg_set_value(reg, r_reg_get(reg, "N", -1), R_TRUE);
	return r_reg_set_value(reg, r_reg_get(reg, "a", -1), dval);
}

int gb_sub_reg(RReg *reg, const char *src)
{
	if(!(reg && src))
		return R_FALSE;
	return gb_sub_const(reg, r_reg_getv(reg, src));
}

int gb_cp_const(RReg *reg, const ut8 src)
{
	if(!reg)
		return R_FALSE;
	ut8 a = r_reg_getv(reg, "a");
	int ret = gb_sub_const(reg, src);
	r_reg_set_value(reg, r_reg_get(reg, "a", -1), a);
	return ret;
}

int gb_cp_reg(RReg *reg, const char *src)
{
	if(!src)
		return R_FALSE;
	return gb_cp_const(reg, r_reg_getv(reg, src));
}

int gb_xor(RReg *reg, const ut8 src){
	if(!reg)
		return R_FALSE;
	r_reg_set_value(reg, r_reg_get(reg, "N", -1), R_FALSE);
	r_reg_set_value(reg, r_reg_get(reg, "H", -1), R_FALSE);
	r_reg_set_value(reg, r_reg_get(reg, "C", -1), R_FALSE);
	if(r_reg_getv(reg, "a") == src)
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_TRUE);
	else
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_FALSE);
	return r_reg_set_value(reg, r_reg_get(reg, "a", -1), r_reg_getv(reg, "a") ^ src);
}

int gb_xor_reg(RReg *reg, const char *src)
{
	if(!(reg && src))
		return R_FALSE;
	return gb_xor(reg, r_reg_getv(reg, src));
}

int gb_swap_reg(RReg *reg, const char *dest)
{
	if(!(reg && dest))
		return R_FALSE;
	ut8 swap = r_reg_getv(reg, dest);
	return r_reg_set_value(reg, r_reg_get(reg, dest, -1), (swap>>4) + (swap<<4));
}

//todo: int gb_swap_ptr(RIO *io, RReg *reg, const ut16 dest)

int gb_res(RReg *reg, const ut8 arg, const char *dest)
{
	if(!(reg && dest))
		return R_FALSE;
	ut8 res = 0x1<<arg;
	ut8 dval = (r_reg_getv(reg, dest) & (~res));
	return r_reg_set_value(reg, r_reg_get(reg, dest, -1), dval);
}

int gb_jmp(RReg *reg, const ut16 dest)
{
	if(!reg)
		return R_FALSE;
	if(dest>0x3fff && dest<0x8000) {						//bankswitches
		r_reg_set_value(reg, r_reg_get(reg, "m", -1), r_reg_getv(reg, "mbc"));
	} else {
		r_reg_set_value(reg, r_reg_get(reg, "m", -1), 0);
	}
	return r_reg_set_value(reg, r_reg_get(reg, "pc", -1), dest);
}

int gb_jmp_cond(RReg *reg, const char *cond, const ut16 dest)
{
	if(!(reg && cond))
		return R_FALSE;
	if(cond[0]=='n') {
		if(r_reg_getv(reg, &cond[1]))
			return R_TRUE;
	} else {
		if(!r_reg_getv(reg, cond))
			return R_TRUE;
	}
	return gb_jmp(reg, dest);
}

int gb_jmp_rel(RReg *reg, const st8 dest)
{
	if(!reg)
		return R_FALSE;
	return r_reg_set_value(reg, r_reg_get(reg, "pc", -1), r_reg_getv(reg, "pc")+dest);
}

int gb_jmp_rel_cond(RReg *reg, const char *cond, const st8 dest)
{
	if(!(reg && cond))
		return R_FALSE;
	if(cond[0]=='n') {
		if(r_reg_getv(reg, &cond[1]))
			return R_TRUE;
	} else {
		if(!r_reg_getv(reg, cond))
			return R_TRUE;
	}
	return gb_jmp_rel(reg, dest);
}

int gb_call(RIO *io, RReg *reg, const ut16 dest)
{
	if(!gb_push(io, reg, "pc"))
		return R_FALSE;
	return gb_jmp(reg, dest);
}

int gb_call_cond(RIO *io, RReg *reg, const char *cond, const ut16 dest)
{
	if(!(io && reg && cond))
		return R_FALSE;
	if(cond[0]=='n') {
		if(r_reg_getv(reg, &cond[1]))
			return R_TRUE;
	} else {
		if(!r_reg_getv(reg, cond))
			return R_TRUE;
	}
	return gb_call(io, reg, dest);
}

int gb_ret(RIO *io, RReg *reg)
{
	if(!(gb_pop(io, reg, "pc")))
		return R_FALSE;
	return gb_jmp(reg, r_reg_getv(reg, "pc"));			//I am lazy
}

int gb_ret_cond(RIO *io, RReg *reg, const char *cond)
{
	if(!(io && reg && cond))
		return R_FALSE;
	if(cond[0]=='n') {
		if(r_reg_getv(reg, &cond[1]))
			return R_TRUE;
	} else {
		if(!r_reg_getv(reg, cond))
			return R_TRUE;
	}
	return gb_ret(io, reg);
}

int gb_reti(RIO *io, RReg *reg)
{
	return (gb_ei(io) && gb_ret(io, reg));
}
