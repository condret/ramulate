#ifndef	SCREEN_H
#define	SCREEN_H

#include <r_types.h>
#include <sdb.h>

typedef struct sdbscreen_desc_t {
	const ut16 xsize;
	const ut16 ysize;
} SdbScreenDesc;

typedef struct sdbscreen_rgb_t {
	ut8 r;
	ut8 g;
	ut8 b;
//	ut8 alpha;
} SdbScreenRgb;

typedef struct sdbscreen_pixel_t {
	ut16 x;
	ut16 y;
	SdbScreenRgb *rgb;
} SdbScreenPixel;

/* --- screen.c ---*/
#define	GET_SCR_FALSE	0xff000000
#define SCR_SET_COLOR_R(rgb)	((rgb->r)<<16)
#define SCR_SET_COLOR_G(rgb)	((rgb->g)<<8)
#define SCR_SET_COLOR_B(rgb)	(rgb->b)
#define SCR_GET_COLOR_R(col)	(col>>16)
#define SCR_GET_COLOR_G(col)	((col>>8)&0xff)
#define SCR_GET_COLOR_B(col)	(col&0xff)

#define SET_RGB(x,y,z,rgb)	{ \
				rgb->r = x;\
				rgb->g = y;\
				rgb->b = z;\
				}
typedef int (*SdbScreenForeachPixelCallback)(void *user, Sdb *screen, ut16 x, ut16 y, SdbScreenRgb *rgb);

/* --- screen/screen.c ---*/
Sdb *sdb_screen_new (SdbScreenDesc *desc);
int sdb_screen_set_pixel (Sdb *screen, ut16 x, ut16 y, int color);
int sdb_screen_get_pixel (Sdb *screen, ut16 x, ut16 y);
int sdb_screen_set_pixel_rgb (Sdb *screen, ut16 x, ut16 y, SdbScreenRgb *rgb);
int sdb_screen_get_pixel_rgb (Sdb *screen, ut16 x, ut16 y, SdbScreenRgb *rgb);
int sdb_screen_foreach_pixel (Sdb *screen, SdbScreenForeachPixelCallback cb, void *user);
int sdb_screen_foreach_pixel_in_range (Sdb *screen, SdbScreenForeachPixelCallback cb, ut16 x, ut16 y, ut16 xf, ut16 yf, void *user);
/* --- special-stuff --- */
inline int sdb_screen_clear_to_color (Sdb *screen, SdbScreenRgb *rgb);
int sdb_screen_draw_line (Sdb  *screen, ut16 x, ut16 y, ut16 xf, ut16 yf, SdbScreenRgb *rgb);
/* --- screen/screen.c ---*/

#endif
