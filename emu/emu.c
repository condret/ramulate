#include <emu.h>
#include <r_reg.h>
#include <r_io.h>
#include <r_asm.h>
#include <r_list.h>
#include <r_util.h>
#include <r_anal.h>
#include <r_lib.h>
#include <sdb.h>

emu *emu_new()
{
	struct emu_t *e = R_NEW0(emu);
	e->reg = r_reg_new();
	e->io = r_io_new();
	e->bin = r_bin_new();
	e->lib = r_lib_new("ramulate_plugin");
	e->plugins = r_list_new();
	e->a = r_asm_new();
	e->op = R_NEW0(RAsmOp);
	e->anal = r_anal_new();
	e->anop = r_anal_op_new();
	e->next_vs_id = 0;
	e->screen = NULL;
	r_lib_add_handler(e->lib, RAMULATE_EMU_PLUGIN, "emulation plugin handler", &emu_plugin_cb, &emu_plugin_cb_end, e);
	return e;
}

void emu_free(emu *e)
{
	if (e->reg == e->anal->reg)
		e->reg = NULL;
	else	r_reg_free(e->reg);
	r_io_free(e->io);
	r_bin_free(e->bin);
	r_lib_free(e->lib);
	r_list_free(e->plugins);
	r_asm_free(e->a);
	r_asm_op_free(e->op);
	r_anal_op_free(e->anop);
	r_anal_free(e->anal);
	if (e->vsections) {
		RListIter *iter;
		VSection *vs;
		r_list_foreach(e->vsections, iter, vs)
			virtual_section_rm_i(e, vs->id);
	}
	r_list_free(e->vsections);
	if (e->screen) sdb_free (e->screen);
	free(e);
}

int emu_step (emu *e, ut8 *buf)
{
	int ret;
	ut64 addr = r_reg_getv (e->reg, r_reg_get_name (e->reg, R_REG_NAME_PC));		//Check Breakboints here: new return stat for that
	if (e->plugin->read) {
		if (e->plugin->min_read_sz)
			e->plugin->read (e, addr, buf, e->plugin->min_read_sz);
		else	e->plugin->read (e, addr, buf, sizeof(int));
	} else {
		if (e->plugin->min_read_sz)
			emu_read (e, addr, buf, e->plugin->min_read_sz);
		else	emu_read (e, addr, buf, sizeof(int));
	}

	if (e->plugin->deps & EMU_PLUGIN_DEP_ASM) {						//only disassemble if it is necessary
		r_asm_set_pc (e->a, addr);
		if (e->plugin->min_read_sz)
			r_asm_disassemble (e->a, e->op, buf, e->plugin->min_read_sz);
		else	r_asm_disassemble (e->a, e->op, buf, sizeof(int));
	}

	if (e->plugin->deps & EMU_PLUGIN_DEP_ANAL) {						//only analize if it is necessary
		if (e->plugin->min_read_sz)
			r_anal_op (e->anal, e->anop, addr, buf, e->plugin->min_read_sz);
		else	r_anal_op (e->anal, e->anop, addr, buf, sizeof(int));
	}

	ret = e->plugin->step (e, buf);

	if (e->plugin->deps & EMU_PLUGIN_DEP_ANAL)
		r_anal_op_fini (e->anop);

	return ret;
}
