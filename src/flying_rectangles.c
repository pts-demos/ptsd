#include <genesis.h>
#include <vdp.h>
#include "flying_rectangles.h"
#include "timer.h"

void load_patterns_image(const u8*, VDPPlan);

// Resolution in 8x8 pixel tiles
#define width 80 
#define height 40

// Should prolly move this in some common header?
#define music_beat 60
#define music_fade_cols 7

u16 scroll_counter = 0;
u16 scroll_speed = 2;
const u16 rows_per_loop = 1;
const u16 cols_per_loop = width;

// Buffer containing tile indices for each tile in the tilemap
u8 tilemap_a[width * height];
u8 tilemap_b[width * height];
u16 colors[music_fade_cols];

// Pairs of patterns that are cycled and rendered on two planes
// They form kind of cube like vertex things

u32 tile_1[8] =
{
	0x11111111,
	0x10000001,
	0x10000001,
	0x10000001,
	0x10000001,
	0x10000001,
	0x10000001,
	0x11111111
};

u32 tile_2[8] =
{
	0x00000001,
	0x00000010,
	0x00000100,
	0x00001000,
	0x00010000,
	0x00100000,
	0x01000000,
	0x10000000
};

u32 tile_3[8] =
{
	0x22222222,
	0x10000002,
	0x10000002,
	0x10000002,
	0x10000002,
	0x10000002,
	0x10000002,
	0x11111112
};

u32 tile_4[8] =
{
	0x00000001,
	0x00000010,
	0x00000100,
	0x00001000,
	0x00010000,
	0x00100000,
	0x01000000,
	0x10000000
};

u32 tile_5[8] =
{
	0x33333333,
	0x10000003,
	0x10000003,
	0x10000003,
	0x10000003,
	0x10000003,
	0x10000003,
	0x11111113
};

u32 tile_6[8] =
{
	0x00000001,
	0x00000010,
	0x00000100,
	0x00001000,
	0x00010000,
	0x00100000,
	0x01000000,
	0x10000000
};

#define rgbToU16(r, g, b) (b << 9) + (g << 5) + (r << 1)

void
flying_rectangles_init(void)
{
    u8 r = 0;
    u8 g = 5;
    u8 b = 2;

    // Create a gradient color palette
    for (int i = 1; i < music_fade_cols; i++)
    {
        b += 1;
        if (b > 7) {
            b = 7;
            g++;
            if (g > 7) {
                g = 7;
                r++;
                if (r > 7) {
                    r = 7;
                }
            }
        }
        colors[i] = rgbToU16(r, g, b);
        VDP_setPaletteColor(i, colors[i]);
    }

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
flying_rectangles(void) {
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

	scroll_counter += scroll_speed;
	VDP_setVerticalScroll(PLAN_A, scroll_counter);
	if (scroll_counter % 10 == 0)
		VDP_setHorizontalScroll(PLAN_A, scroll_counter);

	scroll_counter += 1;
	VDP_setVerticalScroll(PLAN_B, scroll_counter);
	if (scroll_counter % 30 == 0)
		VDP_setHorizontalScroll(PLAN_B, scroll_counter);

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

