#ifndef EMU_H
#define EMU_H

#include <r_reg.h>
#include <r_io.h>
#include <r_bin.h>
#include <r_list.h>
#include <r_lib.h>
#include <r_asm.h>
#include <r_anal.h>
#include <r_types.h>
#include <r_socket.h>
#include <sdb.h>
#include <screen.h>

typedef struct virtual_section_t {
	char name[64];
	ut64 addr;
	ut64 size;
	int rwx;
	ut32 id;
	ut32 linked;
	ut8 *data;
} VSection;


typedef struct emu_t {
	RReg *reg;
	RIO *io;
	RSocket *r2;
	ut64 seek;
	RBin *bin;
	RLib *lib;
	RList *vsections;
	ut32 next_vs_id;
	RList *plugins;
	struct emu_plugin_t *plugin;
	RAsm *a;
	RAsmOp *op;
	RAnal *anal;
	RAnalOp *anop;
	Sdb *screen;
	void *data;
} emu;

enum {
	EMU_PLUGIN_DEP_NONE = 0,
	EMU_PLUGIN_DEP_ASM,
	EMU_PLUGIN_DEP_ANAL
};

enum {
	EMU_STEP_RET_UNKNOWN_OP = 0,
	EMU_STEP_RET_OK,
	EMU_STEP_RET_SEGFAULT,
	EMU_STEP_RET_BP
};				//more ?

#define RAMULATE_EMU_PLUGIN	42
#define RAMULATE_SCREEN_PLUGIN	23

typedef struct emu_plugin_t {
	char *arch;
	char *desc;
	char *license;
	ut8 deps;
	ut8 min_read_sz;
	SdbScreenDesc *screen;
	int (*step)(struct emu_t *e, ut8 *buf);
	int (*set_vs_profile)(struct emu_t *e);
	int (*set_reg_profile)(struct emu_t *e);
	int (*read)(struct emu_t *e, ut64 addr, ut8 *buf, int size);
	int (*handle_user_input)(struct emu_t *e, char input);
	void (*allocate_data)(struct emu_t *e);
	void (*free_data)(void *data);
} EPlugin;

typedef struct emu_edb_command_t {		//rename this stuff
	ut64 job;
	ut64 value;
} edbc;

enum {
	EDB_READ = 0,
	EDB_WRITE,
	EDB_SEEK,
	EDB_DR,
	EDB_DRW,
	EDB_DRP,
	EDB_STEP,
	EDB_CLOSE
};

#define	VS_MAX_NAME_LEN	63

enum {
	VS_NO_LINK,
	VS_IS_LINKED,
	VS_HAS_LINKED
};

enum {
	VS_LIST_ID = 0x1,
	VS_LIST_NAME,
	VS_LIST_ADDR = (0x1<<2),
	VS_LIST_SIZE = (0x1<<3),
	VS_LIST_PERM = (0x1<<4),
	VS_LIST_LINK = (0x1<<5),
	VS_LIST_ALL = (0x1<<6)-1
};

/* --- emu/emu.c --- */
emu *emu_new();
void emu_free(emu *e);
int emu_step(emu *e, ut8 *buf);
/* --- emu/emu.c --- */

/* --- vsection --- */
#define	VS_TO_END(addr,vs)	((vs->addr) + (vs->size) - (addr))
#define VS_TO_BEGIN(addr,vs)	((addr) - (vs->addr))
/* --- emu/vsection.c --- */
int virtual_section_add(emu *e, ut64 addr, ut64 size, int rwx, const char *name);
VSection *virtual_section_get_addr(emu *e, ut64 addr);
VSection *virtual_section_get_name(emu *e, char *name);
VSection *virtual_section_get_i(emu *e, int idx);
VSection *virtual_section_get_next_to_addr(emu *e, ut64 addr);
VSection *virtual_section_get_prev_to_addr(emu *e, ut64 addr);
int virtual_section_rm_i(emu *e, int idx);
int virtual_section_read(VSection *vs, ut64 addr, ut8 *buf, ut64 len);
int virtual_section_write(VSection *vs, ut64 addr, ut8 *buf, ut64 len);
int virtual_section_link(emu *e, VSection *vs_dest, VSection *vs_src);
int virtual_section_unlink_to_size(emu *e, VSection *vs, ut64 size);
inline int virtual_section_unlink_has_linked(emu *e, VSection *vs, ut64 size);
inline int virtual_section_unlink_is_linked(emu *e, VSection *vs, ut64 size);
int virtual_section_unlink(emu *e, VSection *vs);
VSection *virtual_section_resolve_link(emu *e, VSection *vs_src);
int virtual_section_list(emu *e, int mode);
//int virtual_section_split(emu *e, ut64 addr, int id);			TODO
/* --- emu/vsection.c --- */

/* --- emu/e_io.c --- */
int emu_read(emu *e, ut64 addr, ut8 *buf, ut64 len);
int emu_write(emu *e, ut64 addr, ut8 *buf, ut64 len);
int emu_remote(emu *e, ut8 *buf);					//buf is only used for step
/* --- emu/e_io.c --- */

/* --- emu/plugins.c --- */
int emu_add(emu *e, EPlugin *p);
int emu_use(emu *e, char *arch);
void emu_list_plugins(emu *e);
int emu_plugin_cb(struct r_lib_plugin_t *p, void *a, void *b);
int emu_plugin_cb_end(struct r_lib_plugin_t *p, void *a, void *b);
/* --- emu/plugins.c --- */

#endif
