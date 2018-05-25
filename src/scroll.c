#include <genesis.h>
#include <vdp.h>
#include "scroll.h"
#include "timer.h"
#include "greets.h"
#include "prerendered_cube.h"

extern u16 rgbToU16(u8 r, u8 g, u8 b);

/* dummy sentinel value */
static const Image blank;

/* scroller images must be 512x16 pixels */
static const Image *scrolltext[] = {
	&greets_0,
	&greets_1,
	&greets_2,
	&blank,
	NULL,
};
static unsigned img = 0;

static void
load_next_image(void)
{
	/* draw at either 0 or 64 tiles (512px) */
	u16 xpos = (img % 2) * 64;
	VDP_clearTileMapRect(PLAN_A, xpos, 15, 64, 2);
	if (scrolltext[img] == NULL)
		return;
	if (scrolltext[img] != &blank)
		VDP_drawImageEx(PLAN_A, scrolltext[img],
		    TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USERINDEX+100+128*img),
		    xpos, 15, TRUE, TRUE);
	img++;
}

void
scroll_init(void)
{
	u16 col1 = rgbToU16(0,  0, 0);
	u16 col2 = rgbToU16(0,  0, 0);
	u16 col3 = rgbToU16(1,  1, 0);
	u16 col4 = rgbToU16(2,  2, 0);
	u16 col5 = rgbToU16(4,  2, 0);
	u16 col6 = rgbToU16(6,  3, 1);
	u16 col7 = rgbToU16(4,  3, 1);
	u16 col8 = rgbToU16(3,  5, 1);
	u16 col9 = rgbToU16(2,  3, 1);
	u16 col10 = rgbToU16(2, 2, 1);
	u16 col11 = rgbToU16(1, 1, 3);
	u16 col12 = rgbToU16(1, 1, 2);
	u16 col13 = rgbToU16(0, 1, 1);
	u16 col14 = rgbToU16(0, 1, 0);
	u16 col15 = rgbToU16(0, 0, 0);

	VDP_setPlanSize(128, 32);
	VDP_setHorizontalScroll(PLAN_B, 0);

	VDP_setPaletteColor(1, col1);
	VDP_setPaletteColor(2, col2);
	VDP_setPaletteColor(3, col3);
	VDP_setPaletteColor(4, col4);
	VDP_setPaletteColor(5, col5);
	VDP_setPaletteColor(6, col6);
	VDP_setPaletteColor(7, col7);
	VDP_setPaletteColor(8, col8);
	VDP_setPaletteColor(9, col9);
	VDP_setPaletteColor(10, col10);
	VDP_setPaletteColor(11, col11);
	VDP_setPaletteColor(12, col12);
	VDP_setPaletteColor(13, col13);
	VDP_setPaletteColor(14, col14);
	VDP_setPaletteColor(15, col15);

	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

	u8 set_tile;
	u32 empty[8] = {
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000
	};

	for (set_tile = 0; set_tile < 20; set_tile++) {
		VDP_loadTileData((const u32*)empty, set_tile, 1, 0);
	}

	cube_inc_x = 1;
	cube_inc_y = -1;

	load_next_image();
}

u8 render_tile = 1;
u8 tilebuffer[8 * 4];
s16 target_line = 0;
u8 dir = 1;

void
scroll(void) {
	static s16 scroll_count = 0;
	const s16 change_interval = 3;
	static s16 change_count = 0;
	static s16 msg_scrolloffset = 0;
	u16 assign_val = 0;
	s16 linediff;

	change_count++;

	prerendered_cube();

	if (change_count > change_interval) {
		u8 x, y;
		for (y = 0; y < 8; y++) {
			linediff = y - target_line;

			if (linediff < -3 || linediff > 3)
				assign_val = 0x00;
			else {
				assign_val = 10 + linediff;
			}

			for (x = 0; x < 4; x++) {
				tilebuffer[y * 4 + x] = (assign_val << 4) + assign_val;
			}
		}

		change_count = 0;
		render_tile++;
		if (render_tile > 30)
			render_tile = 1;

		if (dir == 1)
			target_line++;
		else
			target_line--;

		if (target_line >= 7)
			dir = -1;
		else if (target_line == 0)
			dir = 1;

		VDP_loadTileData((const u32*)tilebuffer, render_tile, 1, 0);

		u16 first = VDP_getPaletteColor(1);
		u16 col;
		for (int i = 2; i < 15; i++)
		{
			col = VDP_getPaletteColor(i);
			VDP_setPaletteColor(i-1, col);
		}
		VDP_setPaletteColor(14, first);
	}

	VDP_fillTileMapRect(PLAN_B, render_tile, 42 - (scroll_count >> 3), 0, 1, 30);

	scroll_count -= 8;

	VDP_setHorizontalScroll(PLAN_B, scroll_count);
	msg_scrolloffset += 2;
	if (msg_scrolloffset % 512 == 0)
		load_next_image();
	VDP_setHorizontalScroll(PLAN_A, VDP_getScreenWidth()-msg_scrolloffset);
	VDP_waitVSync();
}

