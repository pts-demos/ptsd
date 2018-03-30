#include <genesis.h>
#include <vdp.h>
#include "rotating_patterns.h"
#include "timer.h"
#include "owb_sin.h"

void load_patterns_image(const u8*, VDPPlan);

// Resolution in 8x8 pixel tiles
#define width 80 
#define height 40

// Should prolly move this in some common header?
#define music_beat 60
#define music_fade_cols 8

u16 scroll_counter = 0;
u16 scroll_speed = 2;
u8 tiles = 2;
const u16 rows_per_loop = 1;
const u16 cols_per_loop = width;

// Buffer containing tile indices for each tile in the tilemap
u8 tilemap_a[width * height];
u8 tilemap_b[width * height];
u16 colors[music_fade_cols];

// Pairs of patterns that are rendered on two planes
// If these patterns differ a tiny bit, an interesting
// effect happens when they are being moved around with an offset
u32 tile_1[8] =
{
	0x00010000,
	0x00011000,
	0x00111100,
	0x02211110,
	0x02111111,
	0x02211110,
	0x00111100,
	0x00011000
};

u32 tile_2[8] =
{
	0x00010000,
	0x00111000,
	0x01111100,
	0x22111110,
	0x21111111,
	0x22111110,
	0x01111100,
	0x00111000
};

u32 tile_3[8] =
{
	0x00010000,
	0x00000000,
	0x00111000,
	0x22111110,
	0x22100111,
	0x22111110,
	0x00111000,
	0x00000000
};

u32 tile_4[8] =
{
	0x00010000,
	0x00000000,
	0x00111000,
	0x22111110,
	0x22111111,
	0x22111110,
	0x00111000,
	0x00000000
};

u32 tile_5[8] =
{
	0x12222221,
	0x11000111,
	0x10000001,
	0x00000000,
	0x10000001,
	0x11000111,
	0x12222221,
	0x11111111
};

u32 tile_6[8] =
{
	0x12222221,
	0x11000111,
	0x10000011,
	0x10000011,
	0x10000001,
	0x11000111,
	0x11111111,
	0x12222221
};

#define rgbToU16(r, g, b) (b << 9) + (g << 5) + (r << 1)

void
rotating_patterns_init(void)
{
    // Palette that flashes to white
	colors[0] = rgbToU16(4,  1, 6);
	colors[1] = rgbToU16(4,  2, 6);
	colors[2] = rgbToU16(4,  2, 6);
	colors[3] = rgbToU16(4,  3, 6);
	colors[4] = rgbToU16(5,  4, 6);
	colors[5] = rgbToU16(6,  5, 6);
	colors[6] = rgbToU16(6,  6, 6);
	colors[7] = rgbToU16(7,  7, 7);
	colors[8] = rgbToU16(7,  7, 7);
	colors[9] = rgbToU16(7,  7, 7);

	VDP_setPaletteColor(1, colors[0]);
	VDP_setPaletteColor(2, colors[1]);
    // Note: Sets BG to a color - change it back wnhe needed
	VDP_setPaletteColor(0, colors[8]);

	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

	VDP_loadTileData((const u32*)tile_1, 1, 1, 0);
	VDP_loadTileData((const u32*)tile_2, 2, 1, 0);

	u16 tile_index = 1;
	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			tilemap_a[y * width + x] = tile_index;
			tile_index = (tile_index + 1) % 3;
			tilemap_b[y * width + x] = tile_index;
			tile_index = (tile_index + 1) % 3;
		}
	}

	load_patterns_image(tilemap_a, PLAN_A);
	load_patterns_image(tilemap_b, PLAN_B);
}

void
set_palette(u8 palette_index) {
    VDP_setPaletteColor(1, colors[palette_index]);
    VDP_setPaletteColor(2, colors[palette_index+1]);
}

void
rotating_patterns(void) {
	static int color_flash_time = 0;
    static u8 color_flash_fade = 0;
    static int fade_delay = 4;
    static int fade_ticks = 0;
    static int pattern = 0;

	color_flash_time++;
	if (color_flash_time > music_beat) {
        color_flash_fade = music_fade_cols;
		color_flash_time = 0;
        fade_ticks = 0;
        set_palette(color_flash_fade);

        // It takes a while to load these tiles, but it can be hidden by flashing
        // the screen white so it's not noticed the framerate isn't stable

        if (pattern == 0) {
            VDP_loadTileData((const u32*)tile_1, 1, 1, 0);
            VDP_loadTileData((const u32*)tile_2, 2, 1, 0);
            pattern = 1;
        } else if (pattern == 1) {
            pattern = 2;
            VDP_loadTileData((const u32*)tile_3, 1, 1, 0);
            VDP_loadTileData((const u32*)tile_4, 2, 1, 0);
        } else {
            pattern = 0;
            VDP_loadTileData((const u32*)tile_5, 1, 1, 0);
            VDP_loadTileData((const u32*)tile_6, 2, 1, 0);
        }
	}

    if (color_flash_fade > 0) {
        fade_ticks++;

        if (fade_ticks > fade_delay) {
            fade_ticks = 0;
            set_palette(color_flash_fade);
            color_flash_fade--;
        }
    }

	scroll_counter = (scroll_counter + scroll_speed) % SIN_LEN;

	VDP_setVerticalScroll(PLAN_A, sin(scroll_counter));
	VDP_setHorizontalScroll(PLAN_A, cos(scroll_counter));

    // Offset the second pattern a bit - generates an effect that makes the
    // patterns look as if they stretch
	scroll_counter = (scroll_counter+2 + scroll_speed) % SIN_LEN;

	VDP_setVerticalScroll(PLAN_B, sin(scroll_counter));
	VDP_setHorizontalScroll(PLAN_B, cos(scroll_counter));

	VDP_waitVSync();
}

void load_patterns_image(const u8* src, VDPPlan plan) {
	static u16 temp[width * height];

	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			temp[y * width + x] = TILE_ATTR_FULL(0, 0, 0, 0, src[y * width + x]);
		}
	}

	VDP_setTileMapDataRect(plan, (const u16*)temp, 0, 0, width, height);
}

