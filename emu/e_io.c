#include <emu.h>
#include <r_io.h>
#include <r_reg.h>
#include <r_socket.h>
#include <r_types.h>

int emu_read(emu *e, ut64 addr, ut8 *buf, ut64 len)
{
	VSection *vs;
	ut64 delta;
	ut64 read;
	if (!e->vsections)
		return r_io_read_at(e->io, addr, buf, len);
	while (len) {
		if ((vs = virtual_section_get_addr(e, addr))) {
			delta = addr - vs->addr;
			read = vs->size - delta;
			if (read >= len)
				return virtual_section_read (vs, addr, buf, len);
			virtual_section_read (vs, addr, buf, read);
			len -= read;
			addr += read;
			buf += read;
		}
		if ((vs = virtual_section_get_next_to_addr (e, addr)) && !virtual_section_get_addr(e, addr)) {
			read = vs->addr - addr;
			if (read >= len)
				return r_io_read_at (e->io, addr, buf, len);
			r_io_read_at (e->io, addr, buf, read);
			len -= read;
			buf += read;
			addr += read;
		} else {
			if (!virtual_section_get_addr (e, addr))
				return r_io_read_at (e->io, addr, buf, len);
		}
	}
	return R_TRUE;
}

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

#endif

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

int emu_remote (emu *e, ut8 *buf) {
	ut64 addr = e->seek;
	int size, i, type;
	ut8 rbuf[1024];
	ut8 *ptr;
	edbc com;
	if (!r_socket_is_connected (e->r2))
		return R_FALSE;
	r_socket_read_block (e->r2, (ut8 *)&com, 16);
	switch (com.job) {
		case EDB_READ:
			size = (int)com.value;
			for (i = size; i > 0; i -= 1024) {
				if (i > 1024) {
					emu_read (e, addr, rbuf, 1024);
					r_socket_write (e->r2, rbuf, 1024);
					addr += 1024;
				} else {
					emu_read (e, addr, rbuf, i);
					r_socket_write (e->r2, rbuf, i);
				}
			r_socket_flush (e->r2);
			}
			break;
		case EDB_WRITE:
			for (size = (int)com.value; size > 0; size--) {
				r_socket_read_block (e->r2, rbuf, 1);
				emu_write (e, addr, rbuf, 1);
				addr++;
			}
			break;
		case EDB_SEEK:
			e->seek = com.value;
			break;
		case EDB_DR:
			type = com.value & 0xffffffff;
			size = (com.value>>32) & 0xffffffff;
			ptr = r_reg_get_bytes (e->reg, type, &size);
			r_socket_write (e->r2, (ut8 *)&size, 4);
			r_socket_flush (e->r2);
			r_socket_write (e->r2, ptr, size);
			r_socket_flush (e->r2);
			free (ptr);
			ptr = NULL;
			break;
		case EDB_DRW:
			type = com.value & 0xffffffff;
			size = (com.value>>32) & 0xffffffff;
			ptr = malloc (size);
			r_socket_read_block (e->r2, ptr, size);
			r_reg_set_bytes (e->reg, type, ptr, size);
			free (ptr);
			ptr = NULL;
			break;
		case EDB_DRP:
			size = strlen (e->reg->reg_profile_str);
			r_socket_write (e->r2, (ut8 *)&size, 4);
			r_socket_flush (e->r2);
			r_socket_write (e->r2, e->reg->reg_profile_str, size);
			r_socket_flush (e->r2);
			break;
		case EDB_STEP:
			emu_step (e, buf);
			break;
		case EDB_CLOSE:
		default:
			return R_FALSE;
	}
	return R_TRUE;
}
