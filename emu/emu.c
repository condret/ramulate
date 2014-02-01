#include <emu.h>
#include <r_reg.h>
#include <r_io.h>
#include <r_asm.h>
#include <r_list.h>
#include <r_util.h>

emu *emu_new()
{
	struct emu_t *e = R_NEW0(emu);
	e->reg = r_reg_new();
	e->io = r_io_new();
	e->a = r_asm_new();
	e->op = R_NEW0(RAsmOp);
	e->next_vs_id = 0;
	return e;
}

void emu_free(emu *e)
{
	r_reg_free(e->reg);
	r_io_free(e->io);
	r_asm_free(e->a);
	r_asm_op_free(e->op);
	if(e->vsections) {
		RListIter *iter;
		VSection *vs;
		r_list_foreach(e->vsections, iter, vs)
			virtual_section_rm_i(e, vs->id);
	}
	r_list_free(e->vsections);
	free(e);
}
