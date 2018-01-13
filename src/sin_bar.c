#include <genesis.h>
#include <vdp.h>
#include "sin_bar.h"

/* utils/generate_sin 60 5 20 */
const u8 sines[] = { 12,12,13,14,14,15,16,16,17,17,18,18,18,18,18,19,18,18,18,18,18,17,17,16,16,15,14,14,13,12,12,11,10,9,9,8,7,7,6,6,5,5,5,5,5,5,5,5,5,5,5,6,6,7,7,8,9,9,10,11 };
#define SIN_COUNT (sizeof(sines) / sizeof(sines[0]))
u8 msg_xpos = 0;

const u16 tile1_index = 1;
const u16 tile2_index = 2;
const u16 tile3_index = 3;
const u16 tile4_index = 4;
const u16 tile5_index = 5;

char *greetz[] = {
	"Oispa kaljaa",
	"34C3",
	"Arabuusimiehet",
	"shadez",
	'\0'
};
char **msg = greetz;

// Create some tile buffers
// each element in these arrays represents a color register value
// 0x1111ffff would mean the first 4 pixels use color register 1,
// and the last 4 use color register 15

u32 tile1[8] = {
	0x11111111,
	0x22222222,
	0x33333333,
	0x44444444,
	0x55555555,
	0x66666666,
	0x77777777,
	0x88888888
};

u32 tile2[8] = {
	0x88888888,
	0x99999999,
	0xaaaaaaaa,
	0xbbbbbbbb,
	0xcccccccc,
	0xdddddddd,
	0xeeeeeeee,
	0xffffffff
};

// Instead of creating 4 tiles, could be possible to utilize
// flipped rendering and use the same 2 times

u32 tile3[8] = {
	0xffffffff,
	0xeeeeeeee,
	0xdddddddd,
	0xcccccccc,
	0xbbbbbbbb,
	0xaaaaaaaa,
	0x99999999,
	0x88888888
};

u32 tile4[8] = {
	0x88888888,
	0x77777777,
	0x66666666,
	0x55555555,
	0x44444444,
	0x33333333,
	0x22222222,
	0x11111111,
};

// A tile for clearing parts of the screen
u32 tile5[8] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000
};

/**
  * Converts RGB color values into a single u16 color
  * as supported by the VDP color ram
  * r, g and b must be less than 8
  */
u16 rgbToU16(u8 r, u8 g, u8 b)
{
    // There are 64 * 9 bits of color ram on chip
    // They are accessed as 64 16-bit words
    // each word has the format:
    // ----bbb-ggg-rrr-
    // r = red component
    // g = green component
    // b = blue component
    // as each channel has 3 bits of information, each of them can have 8 unique values (0-7)
    // 8 * 8 * 8 = 512 possible colors

    if (r > 7 || g > 7 || b > 7) {
        return 0;
    }

    u16 out = 0;
    out += (b << 9) + (g << 5) + (r << 1);
    return out;
}

u16 sin_bar_palettes[17];

void
sin_bar_init(void)
{
	// load tiles into VDP memory
	VDP_loadTileData((const u32*)tile1, tile1_index, 1, 0);
	VDP_loadTileData((const u32*)tile2, tile2_index, 1, 0);
	VDP_loadTileData((const u32*)tile3, tile3_index, 1, 0);
	VDP_loadTileData((const u32*)tile4, tile4_index, 1, 0);
	VDP_loadTileData((const u32*)tile5, tile5_index, 1, 0);

	u8 r = 0;
	u8 g = 0;
	u8 b = 0;

	// Create a gradient color palette
	for (int i = 1; i < 16; i++)
	{
		r += 1;
		if (r > 7) {
			r = 7;
			g++;
			if (g > 7) {
				g = 7;
				b++;
				if (b > 6) {
					b = 7;
				}
			}
		}
		u16 vdp_col = rgbToU16(r, g, b);

		// Pre-set these colors in the VDP color ram
		VDP_setPaletteColor(i, vdp_col);
		sin_bar_palettes[i] = vdp_col;
	}
}

int
sin_bar(void) {
	static u8 head_idx = 4;
	static u8 tail_idx = 0;
	static u8 bar_head_y = 0;
	static u8 bar_tail_y = 0;

	u16 old_col;

	head_idx = (head_idx + 1) % SIN_COUNT;
	tail_idx = (tail_idx + 1) % SIN_COUNT;

	/* Move color palettes up one row */
	old_col = sin_bar_palettes[1];
	for (u8 palrow = 1; palrow < 16; palrow++)
	{
		if (palrow < 15)
			sin_bar_palettes[palrow] = sin_bar_palettes[palrow+1];
		else
			sin_bar_palettes[palrow] = old_col;

		VDP_setPaletteColor(palrow, sin_bar_palettes[palrow]);
	}

	u8 old_tail = bar_tail_y;
	bar_head_y = sines[head_idx];
	bar_tail_y = sines[tail_idx];

	/*
	 * Clear the trail. If our tail is moving downward, clear the topmost
	 * line of the bar; if moving upward, clear the bottom line (top+3). If
	 * the tail is not moving do not clear anything.
	 */
	if (old_tail < bar_tail_y)
		VDP_fillTileMapRect(PLAN_A, tile5_index, 0,
		    old_tail, 40, 1);
	else if (old_tail > bar_tail_y)
		VDP_fillTileMapRect(PLAN_A, tile5_index, 0,
		    old_tail+3, 40, 1);

	/*
	 * Draw the bar. Each tile is 8*8px and composes a fourth of the height
	 * of the bar, and we draw them across the entire screen width (ie.
	 * 40*8 pixels)
	 */
	VDP_fillTileMapRect(PLAN_A, tile1_index, 0, bar_head_y, 40, 1);
	VDP_fillTileMapRect(PLAN_A, tile2_index, 0, bar_head_y+1, 40, 1);
	VDP_fillTileMapRect(PLAN_A, tile3_index, 0, bar_head_y+2, 40, 1);
	VDP_fillTileMapRect(PLAN_A, tile4_index, 0, bar_head_y+3, 40, 1);

	VDP_setTextPlan(PLAN_B);
	VDP_setTextPalette(3);
	if (tail_idx % 16 == 0)
		msg_xpos = (msg_xpos + 1) % 40;
	if (tail_idx == SIN_COUNT-1) {
		msg++;
		if (!*msg)
			msg = greetz;
	}
	VDP_drawText(*msg, msg_xpos, bar_head_y+1);
	VDP_waitVSync();
	VDP_clearText(msg_xpos, bar_head_y+1, 40);
	return (0);
}
