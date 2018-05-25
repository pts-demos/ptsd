#include <genesis.h>
#include "sin_wave_data.h"
#include "sin_time_data.h"
#include "pts_math.h"
#include "sin_time_data.h"
#include "sin_wave_data.h"

#define columnsInTile 8
#define rowsInTile 8
#define screenTileWidth 40
#define screenTileWidthQuarter 20
#define screenTileHeight 28
#define screenTileHeightQuarter 14
#define screenPixelWidth 320
#define screenPixelWidthQuarter 160
#define screenPixelHeight 240
#define screenPixelHeightQuarter 120
#define screenPixelHalfX screenPixelWidth / 2
#define screenPixelHalfY screenPixelHeight / 2

u32* wave2_tilebuffer = NULL;
extern u16 rgbToU16(u8 r, u8 g, u8 b);
s16 wave2_scroll = 10;

// distance to the screen center point
u16 wave2_distance_x = 0;
u16 wave2_distance_y = 0;
s16 wave2_pixel_x = 0;
s16 wave2_pixel_y = 0;
u8 wave2_channel_index = 0;
u32 wave2_arrIndex = 0;
u16 wave2_distance = 0;
u8* wave2_sin_wave_data;
u16 wave2_sin_wave_count;
u8* wave2_sin_time_data;
u16 wave2_sin_time_count;

void wave2_fade(void)
{
	MEM_free(wave2_tilebuffer);
    SYS_disableInts();
    VDP_setHInterrupt(FALSE);
    SYS_enableInts();
    VDP_fadeOutAll(60, 0);
    VDP_clearSprites();
    VDP_resetScreen();
    VDP_setHInterrupt(0);
    VDP_setPlanSize(64, 64);
}

void
wave2_init(void)
{
	// Don't allocate this in .text segment as it eats up too much space
	// TODO: free this
	wave2_tilebuffer = MEM_alloc(screenTileWidthQuarter * screenTileHeightQuarter *
		rowsInTile * sizeof(u32));

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    u16 palettes[17];

    u8 r = 0;
    u8 g = 0;
    u8 b = 0;

    // Create a gradient color palette
    for (int i = 1; i < 16; i++)
    {
        b += 1;
        if (b > 7) {
            b = 7;
            g++;
            if (g > 7) {
                g = 7;
                r++;
                if (r > 6) {
                    r = 7;
                }
            }
        }
        palettes[i] = rgbToU16(r, g, b);
        VDP_setPaletteColor(i, palettes[i]);
    }

	// Draw a sequence of tiles that will be updated every frame
    // Tile 0 is background, so start indexing at 1
	// The tiles are drawn so that only the top left quarter of the screen
	// is updated - the rest of the quarters are copied from these tiles
	// so the whole screen gets updated but only quarter of it needs to be
	// computed.
	u16 tileIndex = 0;
	s16 tile_y, tile_x;
	for (tile_y = 0; tile_y < screenTileHeight; tile_y++) {
		for (tile_x = 0; tile_x < screenTileWidth; tile_x++) {
			if (tile_y < screenTileHeightQuarter) {
				if (tile_x < screenTileWidthQuarter) {
					// top left, order is normal
					tileIndex = tile_y * screenTileWidthQuarter + tile_x;
				}
				else {
					// top right, mirror along x axis
					tileIndex = tile_y * screenTileWidthQuarter +
						(screenTileWidthQuarter - (tile_x - screenTileWidthQuarter)) -1;
				}
			} else {
				if (tile_x < screenTileWidthQuarter) {
					// bottom left, flip y
					tileIndex = (screenTileHeight - tile_y -1) * screenTileWidthQuarter +
						tile_x;
				}
				else {
					// bottom right, flip x and y
					tileIndex = (screenTileHeight - tile_y -1) *
						screenTileWidthQuarter + (screenTileWidth - tile_x) -1;
				}
			}
			tileIndex += 1;

			VDP_fillTileMapRect(PLAN_A, tileIndex, tile_x, tile_y, 1, 1);
		}
	}

	wave2_sin_wave_data = get_sin_wave_ptr();
	wave2_sin_wave_count = get_sin_wave_count();
	wave2_sin_time_data = get_sin_time_ptr();
	wave2_sin_time_count = get_sin_time_count();
}

void
wave2(void)
{
    static u16 counter = 0;
    static u16 sin_time = 0;

    static u16 line_to_draw = 0;

    counter++;
    if (counter > wave2_sin_time_count)
        counter = 0;

    wave2_scroll += 1;
	if (counter > wave2_sin_time_count)
		counter = counter % wave2_sin_time_count;
    sin_time = wave2_sin_time_data[counter];

	// As the wave pattern is drawn in the center of screen, we only need to
	// calculate one quarter of the screen - the rest can be duplicated to the
	// other quarters
	static int wave2_loops = 1;
	wave2_loops++;

    for (u32 y = line_to_draw; y < screenTileHeightQuarter; y++)
    {
		// multiply by 8
        wave2_pixel_y = y << 3;
        wave2_distance_y = abs(screenPixelHalfY - wave2_pixel_y) + sin_time;

        for (u32 x = 0; x < screenTileWidthQuarter; x++)
        {
            // to get a moving sine wave at pixel (x,y), the following equation
            // can be used:
            // intensity = |sin(1 / (sqrt( (i*sin(t))^2 + (j*sin(t))^2)))|
            // where
            // 	i, j: the pixel coordinates [0, 1]
            //	t: time
            //  sin: sine function returning [-1, 1]
            //
            // If the equation is reduced to:
            //   intensity = sqrt(i^2 + j^2)
            // A circular gradient is produced
            // Adding sine and scaling terms produces a cyclic wave pattern
            // 
            // this needs to be adapted to:
            // 1. Work without floats (implement sqrt or avoid sqrt in the first place)
            // 2. Work without real-time sine (use look-up table)

			/// multiply by 8
            wave2_pixel_x = x << 3;
            wave2_distance_x = abs(screenPixelHalfX - wave2_pixel_x) + sin_time;

			wave2_distance = silly_sqrt((wave2_distance_x * wave2_distance_x)
				& (wave2_distance_y * wave2_distance_y)) << 1;

			if (wave2_distance > wave2_sin_wave_count)
				wave2_distance = wave2_distance % wave2_sin_wave_count;

            // The distance we work with are [0,160] (half screen width at maximum, height is less and doesn't really matter)
            // To add the time component (the sin inside the sin) we pre-compute a sin table that contains values from 0 to 160
            // Then we add the pixel's distance to the center [0,160] and the sin [0,160] together to get [0,320]
            // Now, assuming we have a look-up table of size 320 we can look up the sine values for each distance

            // assuming there are 8 different red color channels,
            // we can split the 160 different distances to 8 gradient ranges
            // 160 / 8 == 20
            // So a distance of 20 pixels changes the color palette
            // Let's say at the center of the screen red channel is 0 and at the edges it is 7
            // Calculate what color palette to use at a specific distance to the screen center
            //channel_index = distance / 20;
            wave2_channel_index = wave2_sin_wave_data[wave2_distance];
            wave2_arrIndex = (y * screenTileWidth + x) * rowsInTile;

			// This test is necessary for creating a beautiful effect
			// but it's not strictly needed if some artifacts are ok
			// Not clamping this produces some noise when the effect is "zoomed in"
            if (wave2_channel_index > 15)
                wave2_channel_index = 15;

            u32 all_chans = 0;
            all_chans += (wave2_channel_index << 28)
                + (wave2_channel_index << 24)
                + (wave2_channel_index << 20)
                + (wave2_channel_index << 16)
                + (wave2_channel_index << 12)
                + (wave2_channel_index << 8)
                + (wave2_channel_index << 4)
                + (wave2_channel_index);

            for (u32 p = 0; p < 8; p++)
                wave2_tilebuffer[wave2_arrIndex + p] = all_chans;
        }
    }

	// Update the tile data for the top left quarter of the screen
	// As the same tiles are used everywhere else, they will also update
	u16 tileIndex = 1;
    for (u32 y = 0; y < screenTileHeightQuarter; y++)
    {
        for (u32 x = 0; x < screenTileWidthQuarter; x++)
        {
            wave2_arrIndex = (y * screenTileWidth + x) * rowsInTile;
            VDP_loadTileData((const u32*)&wave2_tilebuffer[wave2_arrIndex],
				tileIndex, 1, 0);
            tileIndex++;
        }
    }

    VDP_waitVSync();
}

