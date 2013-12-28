/* ramulate - LGPL - Copyright 2013 - condret@runas-racer.com */

#include <gb_emu.h>
/*
	this file will be deprecated, later there will be no need for it
*/

/* this function was copied from https://github.com/radare/radare2/blob/5fafc517315bce49b47a1d3c2eb0e3cf505c5b31/libr/reg/t/test.c */
void show_regs(struct r_reg_t *reg, int bitsize) {
	RList *reglist;
	RListIter *iter;
	RRegItem *ri;
	eprintf("%d bit registers:\n", bitsize);
	reglist = r_reg_get_list(reg, R_REG_TYPE_GPR);
	r_list_foreach (reglist, iter, ri) {
		if (ri->size == bitsize)
			eprintf(" - %s : 0x%08"PFMT64x"\n", ri->name, r_reg_get_value(reg, ri));
	}
}

void show_gb_flag_regs(RReg *reg) {
	printf("flag registers:\n");
	printf(" - C : 0x%08"PFMT64x"\n",r_reg_get_value(reg, r_reg_get(reg,"C",-1)));
	printf(" - H : 0x%08"PFMT64x"\n",r_reg_get_value(reg, r_reg_get(reg,"H",-1)));
	printf(" - n : 0x%08"PFMT64x"\n",r_reg_get_value(reg, r_reg_get(reg,"n",-1)));
	printf(" - z : 0x%08"PFMT64x"\n",r_reg_get_value(reg, r_reg_get(reg,"z",-1)));
}
