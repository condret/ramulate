#include <emu.h>
#include <r_lib.h>

EPlugin dummy = {
	.arch = "test",
	.desc = "Skelleton test plugin",
	.license = "LGPL3",
	.deps = EMU_PLUGIN_DEP_NONE,
	.min_read_sz = 42,
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
	.data = &dummy
};
