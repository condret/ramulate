#include <screen.h>
#include <emu.h>
#include <gb.h>
#include <r_lib.h>

SdbScreenDesc gb_screen = {
	.xsize = 160,
	.ysize = 144,
};

EPlugin gb_emu = {
	.arch = "gb",
	.desc = "Gameboy emulation plugin",
	.license = "LGPL3",
	.deps = (EMU_PLUGIN_DEP_ASM | EMU_PLUGIN_DEP_ANAL),
	.min_read_sz = 8,
	.screen = &gb_screen,
	.step = &gb_step,
	.set_vs_profile = &gb_set_vs_profile,
	.set_reg_profile = &gb_set_reg_profile,
	.read = NULL,
	.handle_user_input = &gb_input_handler,
	.allocate_data = &gb_data_new,
	.free_data = &gb_data_free,
};

struct r_lib_struct_t ramulate_plugin = {
	.type = RAMULATE_EMU_PLUGIN,
	.data = &gb_emu
};
