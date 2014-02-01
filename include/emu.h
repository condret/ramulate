#include <r_reg.h>
#include <r_io.h>
#include <r_list.h>
#include <r_asm.h>
#include <r_types.h>

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
	RList *vsections;
	ut32 next_vs_id;
//	RList *inverse;		<-- this will enable backwards-execution
	RAsm *a;
	RAsmOp *op;
	void *data;		//<-- storage for arch-specific info, such as mbc-type : must be freed manually
} emu;


#define	VS_MAX_NAME_LEN	63

enum {
	VS_NO_LINK,
	VS_IS_LINKED,
	VS_HAS_LINKED
};


/* --- emu/emu.c --- */
emu *emu_new();
void emu_free(emu *e);
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
//int virtual_section_split(emu *e, ut64 addr, int id);			TODO
/* --- emu/vsection.c --- */


/* --- emu/e_io.c --- */
int emu_read(emu *e, ut64 addr, ut8 *buf, ut64 len);
int emu_write(emu *e, ut64 addr, ut8 *buf, ut64 len);
/* --- emu/e_io.c --- */
