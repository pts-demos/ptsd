#include <genesis.h>
#include <vdp.h>
#include "sin_bar.h"

// Load a pre-generated sine table
#define SIN_COUNT 128
const u8 sines[SIN_COUNT] = { 8,8,9,10,11,11,12,13,13,14,14,15,15,15,15,15,15,15,15,15,15,14,14,13,13,12,12,11,10,9,9,8,7,6,5,5,4,3,3,2,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,2,3,4,5,5,6,7,8,8,9,10,11,11,12,13,13,14,14,15,15,15,15,15,15,15,15,15,15,14,14,13,13,12,12,11,10,9,8,8,7,6,5,5,4,3,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,2,3,3,4,5,5,6,7 };
const u8 *sin_bar_data = sines;
u8 sin_bar_1_index = 17;
u8 sin_bar_2_index = 0;
u8 sin_bar_draw_line_y = 0;
u8 sin_bar_draw_line_y2 = 0;

const u16 tile1_index = 1;
const u16 tile2_index = 2;
const u16 tile3_index = 3;
const u16 tile4_index = 4;
const u16 tile5_index = 5;

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


int sin_bar() {
	// load tiles into VDP memory
	VDP_loadTileData((const u32*)tile1, tile1_index, 1, 0);
	VDP_loadTileData((const u32*)tile2, tile2_index, 1, 0);
	VDP_loadTileData((const u32*)tile3, tile3_index, 1, 0);
	VDP_loadTileData((const u32*)tile4, tile4_index, 1, 0);
	VDP_loadTileData((const u32*)tile5, tile5_index, 1, 0);

	u8 r = 0;
	u8 g = 0;
	u8 b = 0;

	u16 palettes[17];

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
		palettes[i] = vdp_col;
	}

	u16 old_col;
	u8 palrow = 1;

	sin_bar_1_index = (sin_bar_1_index + 1) % SIN_COUNT;
	sin_bar_2_index = (sin_bar_2_index + 1) % SIN_COUNT;

	// Move color palettes up one row
	old_col = palettes[1];
	for (palrow = 1; palrow < 16; palrow++)
	{
		if (palrow < 15)
			palettes[palrow] = palettes[palrow+1];
		else
			palettes[palrow] = old_col;

		VDP_setPaletteColor(palrow, palettes[palrow]);
	}

	// Row where the tiles are placed
	// This line is 8 * sin_bar_draw_line_y pixels
	sin_bar_draw_line_y = sin_bar_data[sin_bar_1_index];
	sin_bar_draw_line_y2 = sin_bar_data[sin_bar_2_index];

	// Avoid drawing the black tile on top of the main effect
	// as that causes a nasty blinking effect
	if (sin_bar_draw_line_y2 > sin_bar_draw_line_y)
	{
		if (sin_bar_draw_line_y2 - sin_bar_draw_line_y < 4)
			sin_bar_draw_line_y2 *= 2;
	}
	else
	{
		if (sin_bar_draw_line_y - sin_bar_draw_line_y2 < 4)
			sin_bar_draw_line_y2 *= 2;
	}

	// Draw full screen width sized tiles that are 8 pixels high

	// The first 4 draws clear 4 * 8 pixels of old data by drawing black over them
	VDP_fillTileMapRect(PLAN_A, tile5_index, 0, sin_bar_draw_line_y2, 40, 1);
	VDP_fillTileMapRect(PLAN_A, tile5_index, 0, sin_bar_draw_line_y2+1, 40, 1);
	VDP_fillTileMapRect(PLAN_A, tile5_index, 0, sin_bar_draw_line_y2+2, 40, 1);
	VDP_fillTileMapRect(PLAN_A, tile5_index, 0, sin_bar_draw_line_y2+3, 40, 1);	

	// These draw the moving bar
	VDP_fillTileMapRect(PLAN_A, tile1_index, 0, sin_bar_draw_line_y, 40, 1);
	VDP_fillTileMapRect(PLAN_A, tile2_index, 0, sin_bar_draw_line_y+1, 40, 1);
	VDP_fillTileMapRect(PLAN_A, tile3_index, 0, sin_bar_draw_line_y+2, 40, 1);
	VDP_fillTileMapRect(PLAN_A, tile4_index, 0, sin_bar_draw_line_y+3, 40, 1);

	return (0);
}

