#include <emu.h>
#include <gb.h>
#include <r_lib.h>

EPlugin gb_emu = {
	.arch = "gb",
	.desc = "Gameboy emulation plugin",
	.license = "LGPL3",
	.deps = (EMU_PLUGIN_DEP_ASM | EMU_PLUGIN_DEP_ANAL),
	.min_read_sz = 8,
	.step = &gb_step,
	.set_vs_profile = &gb_set_vs_profile,
	.set_reg_profile = &gb_set_reg_profile,
	.read = NULL,
	.handle_user_input = NULL,
	.allocate_data = &gb_mbc_new,
	.free_data = &gb_mbc_free,
};

struct r_lib_struct_t ramulate_plugin = {
	.type = RAMULATE_EMU_PLUGIN,
	.data = &gb_emu
};
