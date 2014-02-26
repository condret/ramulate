#include <emu.h>
#include <r_io.h>
#include <r_types.h>

#if 0
int emu_read(emu *e, ut64 addr, ut8 *buf, ut64 len)
{
	if(!e->vsections) {
		r_io_read_at(e->io, addr, buf, len);
		len = 0;
	}
	while(len) {
		if(virtual_section_get_addr(e, addr) && virtual_section_get_addr(e, addr) == virtual_section_get_addr(e, addr + len)) {
			virtual_section_read(virtual_section_get_addr(e, addr),
						addr, buf, len);
			len = 0;
			return R_TRUE;
		}
		if(len && (!virtual_section_get_next_to_addr(e, addr) || (virtual_section_get_next_to_addr(e, addr)->addr - addr) >= len)) {
			r_io_read_at(e->io, addr, buf, len);
			len = 0;
			return R_TRUE;
		}
		if(!virtual_section_get_addr(e, addr) && (virtual_section_get_next_to_addr(e, addr)->addr - addr) < len ) {
			r_io_read_at(e->io, addr, buf, (virtual_section_get_next_to_addr(e, addr)->addr - addr));
			len = len - (virtual_section_get_next_to_addr(e, addr)->addr - addr);
			buf = buf + (virtual_section_get_next_to_addr(e, addr)->addr - addr);
			addr = addr + (virtual_section_get_next_to_addr(e, addr)->addr - addr);
		}
		if(virtual_section_get_addr(e, addr) && virtual_section_get_addr(e, addr) != virtual_section_get_addr(e, addr + len)) {
			virtual_section_read(virtual_section_get_addr(e, addr), addr, buf, VS_TO_END(addr, virtual_section_get_addr(e, addr)));
			len = len - VS_TO_END(addr, virtual_section_get_addr(e, addr));
			buf = buf + VS_TO_END(addr, virtual_section_get_addr(e, addr));
			addr = addr + VS_TO_END(addr, virtual_section_get_addr(e, addr));
		}
	}
	return R_TRUE;
}

int emu_write(emu *e, ut64 addr, ut8 *buf, ut64 len)
{
	if(!e->vsections) {
		r_io_write_at(e->io, addr, buf, len);
		len = 0;
	}
	while(len) {
		if(virtual_section_get_addr(e, addr) && virtual_section_get_addr(e, addr) == virtual_section_get_addr(e, addr + len)) {
			virtual_section_write(virtual_section_get_addr(e, addr),
						addr, buf, len);
			return R_TRUE;
		}
		if((!virtual_section_get_next_to_addr(e, addr) || (virtual_section_get_next_to_addr(e, addr)->addr - addr) >= len)) {		//re-check this please
			eprintf("%08"PFMT64x"\n", addr);
			r_io_write_at(e->io, addr, buf, len);
			return R_TRUE;
		}
		if(!virtual_section_get_addr(e, addr) && (virtual_section_get_next_to_addr(e, addr)->addr - addr) < len) {
			r_io_write_at(e->io, addr, buf, (virtual_section_get_next_to_addr(e, addr)->addr - addr));
			len = len - (virtual_section_get_next_to_addr(e, addr)->addr - addr);
			buf = buf + (virtual_section_get_next_to_addr(e, addr)->addr - addr);
			addr = addr + (virtual_section_get_next_to_addr(e, addr)->addr - addr);
		}
		if(virtual_section_get_addr(e, addr) && virtual_section_get_addr(e, addr) != virtual_section_get_addr(e, addr + len)) {
			virtual_section_write(virtual_section_get_addr(e, addr), addr, buf, VS_TO_END(addr, virtual_section_get_addr(e, addr)));
			len = len - VS_TO_END(addr, virtual_section_get_addr(e, addr));
			buf = buf + VS_TO_END(addr, virtual_section_get_addr(e, addr));
			addr = addr + VS_TO_END(addr, virtual_section_get_addr(e, addr));
		}
	}
	return R_TRUE;
}
#endif					//Refactor us

int emu_read(emu *e, ut64 addr, ut8 *buf, ut64 len)
{
	if (! e->vsections) {
		r_io_read_at (e->io, addr, buf, len);
		return R_TRUE;
	}
	while (len) {
		if (virtual_section_get_addr (e, addr))
			virtual_section_read (virtual_section_get_addr(e, addr), addr, buf, 1);
		else	r_io_read_at (e->io, addr, buf, 1);
		len--;
		buf++;
		addr++;
	}
	return R_TRUE;
}

int emu_write(emu *e, ut64 addr, ut8 *buf, ut64 len)
{
	if (!e->vsections) {
		r_io_write_at (e->io, addr, buf, len);
		return R_TRUE;
	}
	while (len) {
		if (virtual_section_get_addr (e, addr))
			virtual_section_write (virtual_section_get_addr (e, addr), addr, buf, 1);
		else	r_io_write_at (e->io, addr, buf, 1);
		len--;
		buf++;
		addr++;
	}
	return R_TRUE;
}
