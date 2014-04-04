/* ramulate - LGPL - Copyright 2013 - 2014 - condret@runas-racer.com */

#include <emu.h>
#include <gb.h>
#include <r_bin.h>
#include <r_asm.h>
#include <r_io.h>
#include <r_anal.h>

GBmbc *gb_mbc_new(emu *e)
{
	GBmbc *mbc = R_NEW0(GBmbc);
	gb_get_mbc(e->io, mbc);
	return mbc;
}

GBCpuStats *gb_cpu_stats_new (emu *e)
{
	GBCpuStats *cpu = R_NEW0 (GBCpuStats);
	cpu->mbc = gb_mbc_new (e);
	return cpu;
}

void gb_data_new (emu *e)
{
	GBCpuStats *cpu = gb_cpu_stats_new (e);
	e->data = cpu;
}

void gb_cpu_stats_free (GBCpuStats *cpu) {
	gb_mbc_free (cpu->mbc);
	free (cpu);
}

void gb_data_free (void *data)
{
	GBCpuStats *cpu = (GBCpuStats *)data;
	gb_cpu_stats_free (cpu);
}

int gb_set_vs_profile(emu *e)										//we need a parser here in /emu/vsection.c
{
	virtual_section_add(e, 0x8000, 0x2000, 7, "vram");
	virtual_section_add(e, 0xc000, 0x2000, 7, "internal_ram");
	virtual_section_add(e, 0xe000, 0x1dff, 0, "FUCK YOU!");
	virtual_section_add(e, 0xfe00, 0xa0, 7, "OAM");
	virtual_section_add(e, 0xff00, 0x4c, 7, "IO-Ports");
	virtual_section_add(e, 0xff80, 0x7e, 7, "zram");
	virtual_section_add(e, 0xffff, 1, 7, "ie-flg");
	GBCpuStats *cpu;
	cpu = (GBCpuStats *) e->data;
	int i, j;
	switch(cpu->mbc->rambanks) {
		case GB_NO_RAMBANKS:
			i = 0;
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
			switch(cpu->mbc->rambanks) {
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


int gb_write(emu *e, ut64 addr, ut8 *buf, ut32 len)
{
	if(0x2000 <= addr && addr < 0x4000) {
		if(buf[0] == 0x20 || buf[0] == 0x40 || buf[0] == 0x60)
			return r_reg_set_value(e->reg, r_reg_get(e->reg, "mbcrom", -1), 0);
		if(!buf[0])
			return r_reg_set_value(e->reg, r_reg_get(e->reg, "mbcrom", -1), 0);
		return r_reg_set_value(e->reg, r_reg_get(e->reg, "mbcrom", -1), buf[0]-1);
	}
	if(0x4000 <= addr && addr < 0x6000) {
		if(!buf[0])
			return r_reg_set_value(e->reg, r_reg_get(e->reg, "mbcram", -1), 0);
		return r_reg_set_value(e->reg, r_reg_get(e->reg, "mbcram", -1), buf[0]-1);
	}
	if(0xa000 <= addr && addr < 0xc000)
		return emu_write(e, addr + (r_reg_getv(e->reg, "mbcram") << 16), buf, len);
	return emu_write(e, addr, buf, len);
}

int gb_read(emu *e, ut64 addr, ut8 *buf, ut32 len)
{
	if(0x4000 <= addr && addr < 0x8000)
		return emu_read(e, addr + (r_reg_getv(e->reg, "mbcrom") << 16), buf, len);
	if(0xa000 <= addr && addr < 0xc000)
		return emu_read(e, addr + (r_reg_getv(e->reg, "mbcram") << 16), buf, len);
	return emu_read(e, addr, buf, len);
}

int gb_input_handler (emu *e, char input)
{
	GBCpuStats *cpu = (GBCpuStats *)e->data;
	cpu->input = (ut8)input;
	return R_TRUE;
}

int gb_interrupts (emu *e)			//needs a lot of more love
{
	ut8 ien, ifl, tac, tma, tima, ly;
	ut16 pc = r_reg_getv (e->reg, "pc"), sp = r_reg_getv (e->reg, "sp") - 2;
	GBCpuStats *cpu = (GBCpuStats *)e->data;
	gb_read (e, 0xff07, &tac, 1);
	gb_read (e, 0xff06, &tma, 1);
	gb_read (e, 0xff05, &tima, 1);
	gb_read (e, 0xff0f, &ifl, 1);
	gb_read (e, 0xffff, &ien, 1);
	gb_read (e, 0xff44, &ly, 1);
	if (ly != cpu->ly)
		ly = 0;
	cpu->render_ccl += cpu->cycles - cpu->prev_cycles;
	if (cpu->render_ccl >= 456) {
		cpu->render_ccl -= 456;
		ly++;
		if (ly == 153)
			ly = 0;
		cpu->ly = ly;
	}
	gb_write (e, 0xff44, &ly, 1);
	if ((tac&3) != (cpu->tac&3)) {
		cpu->timer_ccl = 0;		//XXX
		cpu->tac = tac;
	}
	if (tac & 4) {
		cpu->timer_ccl += cpu->cycles - cpu->prev_cycles;
		switch (tac & 3) {
			case 0:
				while (cpu->timer_ccl >= 256) {
					tima = (tima + 1) & 0xff;
					cpu->timer_ccl -= 256;
				}
				break;
			case 1:
				while (cpu->timer_ccl >= 4) {		//check below, if he new value is smaler than the new one, if so, add the content of tma and throw interrupt
					tima = (tima + 1) & 0xff;
					cpu->timer_ccl -= 4;
				}
				break;
			case 2:
				while (cpu->timer_ccl >= 16) {
					tima = (tima + 1) & 0xff;
					cpu->timer_ccl -= 16;
				}
				break;
			case 3:
				while (cpu->timer_ccl >= 64) {
					tima = (tima +1) & 0xff;
					cpu->timer_ccl -= 64;
				}
		}
	}
	if (cpu->input != cpu->prev_input) {
		if ((ien & 16) && r_reg_getv (e->reg, "ime")) {
			ifl = 16;
			gb_write (e, 0xff0f, &ifl, 1);		//how to reset this?
			r_reg_setv (e->reg, "ime", R_FALSE);
			gb_write (e, (ut64)sp, (ut8 *)&pc, 2);	//manual push
			r_reg_setv (e->reg, "mpc", GB_INT_JP);
			r_reg_setv (e->reg, "sp", sp);
			cpu->interruptlevel++;
		}
		cpu->input = cpu->prev_input;
	}
	if (cpu->tima > tima) {
		tima = (tima+tma) & 0xff;
		if ((ien & 4) && r_reg_getv (e->reg, "ime")) {
			ifl = 4;
			gb_write (e, 0xff0f, &ifl, 1);
			r_reg_setv (e->reg, "ime", R_FALSE);
			gb_write (e, (ut64)sp, (ut8 *)&pc, 2);
			r_reg_setv (e->reg, "mpc", GB_INT_TIM);
			r_reg_setv (e->reg, "sp", sp);
			cpu->interruptlevel++;
		}
	}
	if (ly > 144) {						//vblank
		if ((ien & 1) && r_reg_getv (e->reg, "ime")) {
			ifl = 1;
			gb_write (e, 0xff0f, &ifl, 1);
			r_reg_setv (e->reg, "ime", R_FALSE);
			gb_write (e, (ut64)sp, (ut8 *)&pc, 2);
			r_reg_setv (e->reg, "mpc", GB_INT_VBL);
			r_reg_setv (e->reg, "sp", sp);
			cpu->interruptlevel++;
		}
	}
	cpu->tima = tima;
	gb_write (e, 0xff05, &tima, 1);
	return R_TRUE;
}


int gb_step (emu *e, ut8 *buf)
{
	int ret = R_FALSE;
	GBCpuStats *cpu = (GBCpuStats *)e->data;
	cpu->cycles += e->anop->cycles;
	r_reg_set_value (e->reg, r_reg_get (e->reg, "pc", -1), r_reg_getv(e->reg, "pc") + e->op->size);
	switch (e->anop->type) {
		case R_ANAL_OP_TYPE_NOP:
			ret = R_TRUE;
		case R_ANAL_OP_TYPE_TRAP:
		case R_ANAL_OP_TYPE_ILL:
			break;
		case R_ANAL_OP_TYPE_MOV:
			ret = gb_mov (e);
			break;
		case R_ANAL_OP_TYPE_PUSH:
			ret = gb_push (e);
			break;
		case R_ANAL_OP_TYPE_POP:
			ret = gb_pop (e);
			break;
		case R_ANAL_OP_TYPE_JMP:
			ret = gb_jmp (e);
			break;
		case R_ANAL_OP_TYPE_CJMP:
			ret = gb_cjmp (e);
			break;
		case R_ANAL_OP_TYPE_UJMP:
			ret = gb_ujmp (e);
			break;
		case R_ANAL_OP_TYPE_UCALL:
		case R_ANAL_OP_TYPE_CALL:
			ret = gb_call (e);
			break;
		case R_ANAL_OP_TYPE_CCALL:
			ret = gb_ccall (e);
			break;
		case R_ANAL_OP_TYPE_RET:
			ret = gb_ret (e);
			break;
		case R_ANAL_OP_TYPE_CRET:
			ret = gb_cret (e);
			break;
		case R_ANAL_OP_TYPE_CMP:
			ret = gb_cp (e);
			break;
		case R_ANAL_OP_TYPE_SUB:
			ret = gb_sub (e);
			break;
		case R_ANAL_OP_TYPE_ADD:
			ret = gb_add (e);
			break;
		case R_ANAL_OP_TYPE_XOR:
			ret = gb_xor (e);
			break;
		case R_ANAL_OP_TYPE_AND:
			ret = gb_and (e);
			break;
		case R_ANAL_OP_TYPE_OR:
			ret = gb_or (e);
			break;
		case R_ANAL_OP_TYPE_ACMP:
			ret = gb_bit (e);
			break;
		case R_ANAL_OP_TYPE_ROL:
			ret = gb_rol (e);
			break;
		case R_ANAL_OP_TYPE_STORE:
			ret = gb_store (e);
			break;
		case R_ANAL_OP_TYPE_LOAD:
			ret = gb_load (e);
			break;
	}
	gb_interrupts (e);
	cpu->cycles &= 0xffffff;	//prevent overflows
	cpu->prev_cycles = cpu->cycles;
	return ret;
}
