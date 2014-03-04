#include <emu.h>
#include <r_lib.h>

EPlugin snes_emu = {
	.arch = "snes",
	.desc = "Snes emulation plugin - TODO",
	.license = "LGPL3",
	.deps = (EMU_PLUGIN_DEP_ASM | EMU_PLUGIN_DEP_ANAL),
	.min_read_sz = 10,
	.screen = NULL,
	.step = NULL,
	.set_vs_profile = NULL,
	.set_reg_profile = NULL,
	.read = NULL,
	.handle_user_input = NULL,
	.allocate_data = NULL,
	.free_data = NULL,
};

struct r_lib_struct_t ramulate_plugin = {
	.type = RAMULATE_EMU_PLUGIN,
	.data = &snes_emu
};
