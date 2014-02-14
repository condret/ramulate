#include <emu.h>
#include <r_list.h>
#include <r_types.h>
#include <r_lib.h>
#include <stdio.h>

int emu_add(emu *e, EPlugin *p)
{
	if(!e || !p)
		return R_FALSE;
	r_list_append(e->plugins, p);
	return R_TRUE;
}

int emu_use(emu *e, char *arch)
{
	EPlugin *p;
	RListIter *iter;
	if (e && arch) {
		r_list_foreach (e->plugins, iter, p) {
			if (!strcmp (p->arch, arch)) {
				e->plugin = p;
				return R_TRUE;
			}
		}
	}
	return R_FALSE;
}

void emu_list_plugins(emu *e)
{
	RListIter *iter;
	EPlugin *p;
	if(!e || !e->plugins)
	return;
	r_list_foreach(e->plugins, iter, p)
		printf("%s\t%s\n",p->arch, p->desc);
}


int emu_plugin_cb(struct r_lib_plugin_t *p, void *a, void *b)
{
	emu *e;
	e = (emu *)a;
	emu_add (e, (EPlugin *)b);
	return R_TRUE;
}

int emu_plugin_cb_end(struct r_lib_plugin_t *p, void *a, void *b) { return 0; }
