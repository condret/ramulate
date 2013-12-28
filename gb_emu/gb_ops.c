/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <gb_ops.h>
#include <r_asm.h>
#include <r_io.h>		//we need rio-cache here
#include <r_reg.h>

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

int gb_inc(RReg *reg, const char *dest)							//inc
{
	if(!(reg && dest))
		return R_FALSE;
	return r_reg_set_value(reg, r_reg_get(reg, dest, -1), r_reg_getv(reg, dest)-1);
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
	int ret = r_reg_set_value(reg, r_reg_get(reg, dest, -1), r_reg_getv(reg, dest) + r_reg_getv(reg, src));
	if(r_reg_getv(reg, dest))
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_FALSE);
	else
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_TRUE);
	return ret;
}

int gb_sub_reg(RReg *reg, const char *src)
{
	if(!(reg && src))
		return R_FALSE;
	ut8 dval = r_reg_getv(reg, "a") - r_reg_getv(reg, src);
	if(dval)
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_FALSE);
	else
		r_reg_set_value(reg, r_reg_get(reg, "Z", -1), R_TRUE);
	r_reg_set_value(reg, r_reg_get(reg, "N", -1), R_TRUE);
	return r_reg_set_value(reg, r_reg_get(reg, "a", -1), dval);
}

int gb_cp_reg(RReg *reg, const char *src)
{
	ut8 a = r_reg_getv(reg, "a");
	int ret = gb_sub_reg(reg, src);
	r_reg_set_value(reg, r_reg_get(reg, "a", -1), a);
	return ret;
}

int gb_swap_reg(RReg *reg, const char *dest)
{
	if(!(reg && dest))
		return R_FALSE;
	ut8 swap = r_reg_getv(reg, dest);
	return r_reg_set_value(reg, r_reg_get(reg, dest, -1), (swap>>4) + (swap<<4));
}

int gb_call_jmp(RReg *reg, const ut16 dest)
{
	if(!reg)
		return R_FALSE;
	if(dest>0x3fff) {								//bankswitches
		r_reg_set_value(reg, r_reg_get(reg, "m", -1), r_reg_getv(reg, "mbc"));
	} else {
		r_reg_set_value(reg, r_reg_get(reg, "m", -1), 0);
	}
	return r_reg_set_value(reg, r_reg_get(reg, "pc", -1), dest);
}

int gb_jmp_rel(RReg* reg, const st8 dest)
{
	if(!reg)
		return R_FALSE;
	return r_reg_set_value(reg, r_reg_get(reg, "pc", -1), r_reg_getv(reg, "pc")+dest);
}
