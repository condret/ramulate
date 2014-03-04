#include <screen.h>
#include <stdio.h>
#include <unistd.h>
#include <r_types.h>
#include <sdb.h>

Sdb *sdb_screen_new (SdbScreenDesc *desc)
{
	Sdb *screen = sdb_new(NULL, "", 0);
	int x, y;
	char *key = malloc (32);
	sdb_num_set (screen, "xsize", desc->xsize, 0);
	sdb_num_set (screen, "ysize", desc->ysize, 0);
	for (x = 0; x < desc->xsize; x++) {
		snprintf (key, 31, "p%d", x);
		for (y = 0; y <= desc->ysize; y++) {
			sdb_array_set_num (screen, key, y, 0, 0);
		}
	}
	if (!sdb_num_exists (screen, "xsize")) printf ("wait, what?!!\n");
	free (key);
	return screen;
}


int sdb_screen_set_pixel (Sdb *screen, ut16 x, ut16 y, int color)
{
	if (!screen || !sdb_num_exists (screen, "xsize") || !sdb_num_exists (screen, "ysize"))
		return R_FALSE;
	if (sdb_num_get (screen, "xsize", 0) <= x || sdb_num_get (screen, "ysize", 0) <= y)
		return R_FALSE;
	char key[32];
	snprintf (key, 31, "p%d", x);
	return sdb_array_set_num (screen, key, y, color, 0);
}

int sdb_screen_get_pixel (Sdb *screen, ut16 x, ut16 y)
{
	if (!screen || !sdb_num_exists (screen, "xsize") || !sdb_num_exists (screen, "ysize"))
		return GET_SCR_FALSE;
	if (sdb_num_get (screen, "xsize", 0) <= x || sdb_num_get (screen, "ysize", 0) <= y)
		return GET_SCR_FALSE;
	char key[32];
	snprintf (key, 31, "p%d", x);
	return sdb_array_get_num (screen, key, y, 0);
}
int sdb_screen_set_pixel_rgb (Sdb *screen, ut16 x, ut16 y, SdbScreenRgb *rgb)
{
	if (!rgb)
		return R_FALSE;
	int color = SCR_SET_COLOR_R (rgb);
	color += SCR_SET_COLOR_G (rgb);
	color += SCR_SET_COLOR_B (rgb);
	return sdb_screen_set_pixel (screen, x, y, color);
}
int sdb_screen_get_pixel_rgb (Sdb *screen, ut16 x, ut16 y, SdbScreenRgb *rgb)
{
	if (!rgb)
		return R_FALSE;
	int col = sdb_screen_get_pixel (screen, x, y);
	if (col == GET_SCR_FALSE)
		return R_FALSE;
	rgb->r = SCR_GET_COLOR_R (col);
	rgb->g = SCR_GET_COLOR_G (col);
	rgb->b = SCR_GET_COLOR_B (col);
	return R_TRUE;
}

int sdb_screen_foreach_pixel (Sdb *screen, SdbScreenForeachPixelCallback cb, void *user)
{
	ut16 x, y, xf, yf;
	int ret, col;
	char *key;
	SdbScreenRgb *rgb;
	if (!screen || !cb || !sdb_exists (screen, "xsize") || !sdb_exists (screen, "ysize"))
		return R_FALSE;
	rgb = R_NEW0 (SdbScreenRgb);
	key = malloc (32);
	xf = (ut16)sdb_num_get (screen, "xsize", 0);
	yf = (ut16)sdb_num_get (screen, "ysize", 0);
	for (x = 0; x < xf; x++) {
		snprintf (key, 31, "p%d", x);
		for (y = 0; y < yf; y++) {
			col = (int)sdb_array_get_num (screen, key, y, 0);
			rgb->r = SCR_GET_COLOR_R (col);
			rgb->g = SCR_GET_COLOR_G (col);
			rgb->b = SCR_GET_COLOR_B (col);
			if (!(ret = cb (user, screen, x, y, rgb)))
				goto fail;
		}
	}
	fail:
	free (key);
	free (rgb);
	return ret;
}

int sdb_screen_foreach_pixel_in_range (Sdb *screen, SdbScreenForeachPixelCallback cb, ut16 x, ut16 y, ut16 xf, ut16 yf, void *user)
{
	ut16 b;
	int ret, col;
	char *key;
	SdbScreenRgb *rgb;
	if (!screen || !cb || !sdb_exists (screen, "xsize") || !sdb_exists (screen, "ysize"))
		return R_FALSE;
	if ((x || xf) >= sdb_num_get (screen, "xsize", 0))
		return R_FALSE;
	if ((y || yf) >= sdb_num_get (screen, "ysize", 0))
		return R_FALSE;
	if (x > xf) {
		b = x;
		x = xf;
		xf = b;
	}
	b = y;
	if (y > yf) {
		y = yf;
		yf = b;
	}
	xf++;
	yf++;
	rgb = R_NEW0 (SdbScreenRgb);
	key = malloc (32);
	for (; x < xf; x++) {
		snprintf (key, 31, "p%d", x);
		for (y = b; y < yf; y++) {
			col = (int)sdb_array_get_num (screen, key, y, 0);
			rgb->r = SCR_GET_COLOR_R (col);
			rgb->g = SCR_GET_COLOR_G (col);
			rgb->b = SCR_GET_COLOR_B (col);
			if (!(ret = cb (user, screen, x, y, rgb)))
				goto fail_range;
		}
	}
	fail_range:
	free (key);
	free (rgb);
	return ret;
}

int clear_to_color_cb (void *user, Sdb *screen, ut16 x, ut16 y, SdbScreenRgb *unused)
{
	SdbScreenRgb *rgb = (SdbScreenRgb *) user;
	return sdb_screen_set_pixel_rgb (screen, x, y, rgb);
}

inline int sdb_screen_clear_to_color (Sdb *screen, SdbScreenRgb *rgb)
{
	return sdb_screen_foreach_pixel (screen, &clear_to_color_cb, rgb);
}

int sdb_screen_draw_line (Sdb *screen, ut16 x, ut16 y, ut16 xf, ut16 yf, SdbScreenRgb *rgb)
{
	float m, fy = (float)y, fx = (float)x, fyf = (float)yf, fxf = (float)xf;
	st8 v;
	int ret;
	if (!screen || !rgb || !sdb_exists (screen, "xsize") || !sdb_exists (screen, "ysize"))
		return R_FALSE;
	if ((x || xf) >= sdb_num_get (screen, "xsize", 0))
		return R_FALSE;
	if ((y || yf) >= sdb_num_get (screen, "ysize", 0))
		return R_FALSE;
	m = (fyf-fy)/(fxf-fx);
	if (xf > x)	v = 1;
	else		v = -1;
	if (!(ret = sdb_screen_set_pixel_rgb (screen, x, y, rgb)))
		goto fail_line;
	while (x!=xf) {
		x = x + v;
		fy += v*m;
		if (!(ret = sdb_screen_set_pixel_rgb (screen, x, (ut16)(fy + 0.5), rgb)))
			goto fail_line;
	}
	fail_line:
	return ret;
}
