#include <emu.h>
#include <r_reg.h>
#include <r_io.h>
#include <r_asm.h>
#include <r_list.h>
#include <r_util.h>
#include <r_anal.h>
#include <r_lib.h>

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
	if(e->vsections) {
		RListIter *iter;
		VSection *vs;
		r_list_foreach(e->vsections, iter, vs)
			virtual_section_rm_i(e, vs->id);
	}
	r_list_free(e->vsections);
	free(e);
}
