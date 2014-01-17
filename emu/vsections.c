#include <emu.h>
#include <stdlib.h>
#include <string.h>
#include <r_list.h>
#include <r_types.h>
//#include <r_utils.h>

int virtual_section_add(emu *e, ut64 addr, ut64 size, int rwx, const char *name)
{
	if(size == 0 || size > 0xf0000000) {
		eprintf("Invalid size (0x%08"PFMT64x") for vsection '%s' at 0x%08"PFMT64x"\n",
			size, name, addr);
		return R_FALSE;
	}
	if(!e)
		return R_FALSE;
	if(!e->vsections)
		e->vsections = r_list_new();
	struct virtual_section_t *vsection = R_NEW (VSection);
	vsection->addr = addr;
	vsection->size = size;
	vsection->rwx = rwx;
	strncpy (vsection->name, name, VS_MAX_NAME_LEN);
	vsection->name[63] = '\0';
	vsection->data = malloc(size);
	if(!vsection->data) {
		eprintf("vsection: cannot malloc vsection.%s for 0x%08"PFMT64x"\n",
			vsection->name, vsection->addr);
		return R_FALSE;
	}
	vsection->id = e->next_vs_id++;
	vsection->linked = VS_NO_LINK;
	r_list_append(e->vsections, vsection);
	return R_TRUE;
}

VSection *virtual_section_get_addr(emu *e, ut64 addr)
{
	if(!e || !e->vsections)
		return NULL;
	RListIter *iter;
	VSection *vs;
	r_list_foreach (e->vsections, iter, vs) {
		if( (vs->addr <= addr) && (addr <= (vs->addr + vs->size)))
			return vs;
	}
	return NULL;
}

VSection *virtual_section_get_name(emu *e, char *name)
{
	if(!e || !e->vsections)
		return NULL;
	RListIter *iter;
	VSection *vs;
	r_list_foreach (e->vsections, iter, vs) {
		if(!strcmp (vs->name, name))
			return vs;
	}
	return NULL;
}

VSection *virtual_section_get_i(emu *e, int idx)
{
	if(!e || !e->vsections)
		return NULL;
	RListIter *iter;
	VSection *vs;
	r_list_foreach (e->vsections, iter, vs) {
		if(idx == vs->id)
			return vs;
	}
	return NULL;
}


int virtual_section_rm_i(emu *e, int idx)
{
	VSection *vs = virtual_section_get_i (e, idx);
	if(vs) {
		switch(vs->linked) {
			case VS_NO_LINK:
				free(vs->data);
				break;
			case VS_IS_LINKED:
				virtual_section_unlink_is_linked(e, vs, 0);
				break;
			case VS_HAS_LINKED:
				virtual_section_unlink_has_linked(e, vs, 0);
				break;
		}
		r_list_del_n(e->vsections, idx);
	}
	return R_TRUE;
}

int virtual_section_read(VSection *vs, ut64 addr, ut8 *buf, ut64 len)
{
	if(!vs || !vs->data)
		return R_FALSE;
	if((vs->addr > addr || (vs->addr + vs->size) < (addr +len))) {
		eprintf("Memory to read(0x%08"PFMT64x" - 0x%08"PFMT64x") is not in vsection.%s(0x%08"PFMT64x" - 0x%08"PFMT64x")\n",
			addr, addr + len, vs->name, vs->addr, vs->addr + vs->size);
		return R_FALSE;
	}
	memcpy(buf, vs->data + addr - vs->addr, len);
	return R_TRUE;
}

int virtual_section_write(VSection *vs, ut64 addr, ut8 *buf, ut64 len)
{
	if(!vs || !vs->data)
		return R_FALSE;
	if((vs->addr > addr || (vs->addr + vs->size) < (addr +len))) {
		eprintf("Memory to write(0x%08"PFMT64x" - 0x%08"PFMT64x") is not in vsection.%s(0x%08"PFMT64x" - 0x%08"PFMT64x")\n",
			vs->addr, vs->addr + vs->size, vs->name, addr, addr + len);
		return R_FALSE;
	}
	memcpy(vs->data + addr - vs->addr, buf, len);
	return R_TRUE;
}

int virtual_section_link(emu *e, VSection *vs_dest, VSection *vs_src)
{
	if(!e || !e->vsections)
		return R_FALSE;
	if(vs_dest->linked == VS_NO_LINK)
		vs_dest->linked = VS_HAS_LINKED;
	if(vs_src->linked == VS_HAS_LINKED) {
		RListIter *iter;
		VSection *vs;
		r_list_foreach(e->vsections, iter, vs) {
			if(vs->data == vs_src->data && vs != vs_src) {
				vs->size = vs_dest->size;
				vs->data = vs_dest->data;
			}
		}
	}
	if(vs_src->linked == VS_HAS_LINKED || vs_src->linked == VS_NO_LINK)
		free(vs_src->data);
	vs_src->linked = VS_IS_LINKED;
	vs_src->size = vs_dest->size;
	vs_src->data = vs_dest->data;
	return R_TRUE;
}

int virtual_section_unlink_to_size(emu *e, VSection *vs, ut64 size)
{
	if(!vs)
		return R_FALSE;
	if(vs->linked == VS_NO_LINK) {
		eprintf("vsection.%s is not linked\n", vs->name);
		return R_FALSE;
	}
	if(size == 0 || size > 0xf0000000) {
		eprintf("Invalid size (0x%08"PFMT64x") for vsection '%s' at 0x%08"PFMT64x"\n",
		size, vs->name, vs->addr);
		return R_FALSE;
	}
	int ret;
	if(vs->linked == VS_IS_LINKED)
		ret = virtual_section_unlink_is_linked(e, vs, size);
	else
		ret = virtual_section_unlink_has_linked(e, vs, size);
	if(!vs->data) {
		eprintf("vsection: unlink to size 0x%08"PFMT64x" failed\n"
			"cannot malloc vsection.%s for 0x%08"PFMT64x"\n",
			size, vs->name, vs->addr);
		return R_FALSE;
	}
	return ret;
}

inline int virtual_section_unlink_is_linked(emu *e, VSection *vs, ut64 size)
{
	if(!e || !e->vsections)
		return R_FALSE;
	VSection *vsb, *vsd;
	RListIter *iter;
	ut8 *n = malloc(size);
	r_list_foreach(e->vsections, iter, vsb) {
		if(vs != vsb && vs->data == vsb->data && vsb->linked == VS_IS_LINKED) {
			vs->data = n;
			vs->size = size;
			vs->linked = VS_NO_LINK;
			return R_TRUE;
		}
		if(vs->data == vsb->data && vsb->linked == VS_HAS_LINKED)
			vsd = vsb;
	}
	vs->linked = VS_NO_LINK;
	vsd->linked = VS_NO_LINK;					//<-- if there was only 1 link
	vs->size = size;
	vs->data = n;
	return R_TRUE;
}

inline int virtual_section_unlink_has_linked(emu *e, VSection *vs, ut64 size)
{
	if(!e || !e->vsections)
		return R_FALSE;
	VSection *vsb, *vsd;
	RListIter *iter;
	ut32 c = 0;
	ut8 *n = malloc(size);
	r_list_foreach(e->vsections, iter, vsb) {
		if(vs->data == vsb->data) {
			c++;
			vsd=vsb;
		}
		if(c==2) {
			vsd->linked = VS_HAS_LINKED;
			vs->linked = VS_NO_LINK;
			vs->data = n;
			vs->size = size;
			return R_TRUE;
		}
	}
	if(c) {
		vs->linked = vsd->linked = VS_NO_LINK;
		vs->data = n;
		vs->size = size;
		return R_TRUE;
	}
	free(n);								//<-- if you're here, something went wrong
	vs->linked = VS_NO_LINK;
	eprintf("oops\n");
	return R_FALSE;
}

int virtual_section_unlink(emu *e, VSection *vs)		//use makro here
{
	return virtual_section_unlink_to_size(e, vs, vs->size);
}

VSection *virtual_section_resolve_link(emu *e, VSection *vs_src)
{
	if(!vs_src) {
		eprintf("vsection: cannot resolve link: ono src\n");
		return NULL;
	}
	if(vs_src->linked != VS_IS_LINKED) {
		eprintf("vsection.%s is not linked\n", vs_src->name);
		return NULL;
	}
	if(!e || !e->vsections) {
		eprintf("vsection: cannot resolve link for vsection.%s:\nno list\n", vs_src->name);
		return NULL;
	}
	RListIter *iter;
	VSection *vs;
	r_list_foreach(e->vsections, iter, vs) {
		if(vs->linked == VS_HAS_LINKED && vs->data == vs_src->data)
			return vs;
	}
	eprintf("vsection: cannot resolve link for vsection.%s\n"
		"link-destination was unusually removed\n",
		vs_src->name);
	return NULL;
}
