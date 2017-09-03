#include <genesis.h>
#include "sin_wave_data.h"
#include "sin_time_data.h"

#define columnsInTile 8
#define rowsInTile 8
#define screenTileWidth 40
#define screenTileHeight 28
#define screenPixelWidth 320
#define screenPixelHeight 240
#define screenPixelHalfX screenPixelWidth / 2
#define screenPixelHalfY screenPixelHeight / 2

u32 tilebuffer[screenTileWidth * screenTileHeight * rowsInTile];

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

// Found on the internet
u16 silly_sqrt(u32 x)
{
	u16 res = 0;
	u16 add = 0x8000;
	u8 i;
	for (i = 0; i < 16; i++)
	{
		u16 temp = res | add;
		u32 g2 = temp * temp;
		if (x >= g2)
		{
			res = temp;
		}
		add >>= 1;
	}
	return res;
}

int main()
{
	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    u16 palettes[17];

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
        palettes[i] = rgbToU16(r, g, b);
        VDP_setPaletteColor(i, palettes[i]);
    }


	s16 scroll = 10;

	// distance to the screen center point
	u16 distance_x_a = 0;
	u16 distance_y_a = 0;

	u16 distance_x_b = 0;
	u16 distance_y_b = 0;
	u16 distance_a = 0;
	u16 distance_b = 0;
	u16 distance = 0;

	u16 wave_origin_a_x = 10;
	u16 wave_origin_a_y = 10;

	u16 wave_origin_b_x = 300;
	u16 wave_origin_b_y = 200;

	s16 pixel_x = 0;
	s16 pixel_y = 0;
	u8 channel_index = 0;
	u32 arrIndex = 0;

	// Tile 0 is backgroud, so start indexing at 1
	u16 tileIndex = 1;
	for (u32 y = 0; y < screenTileHeight; y++)
	{
		for (u32 x = 0; x < screenTileWidth; x++)
		{
			arrIndex = (y * screenTileWidth + x) * rowsInTile;
			VDP_loadTileData((const u32*)&tilebuffer[arrIndex], tileIndex, 1, 0);
			tileIndex++;
		}
	}

	u16 counter = 0;
	u16 sin_time = 0;

	u16 line_to_draw = 0;
	u16 lines_per_loop = screenTileHeight;

	while (1)
	{
		counter++;
		if (counter > SIN_TIME_COUNT)
			counter = 0;

		scroll += 1;
		sin_time = SIN_TIME_DATA[counter];

		for (u32 y = line_to_draw; y < line_to_draw + lines_per_loop; y++)
		{
			pixel_y = y * 8;
			distance_y_a = abs(wave_origin_a_y - pixel_y) + sin_time;
			distance_y_b = abs(wave_origin_b_y - pixel_y) + sin_time;

			for (u32 x = 0; x < screenTileWidth; x++)
			{

				// to get a moving sine wave at pixel (x,y), the following equation can be used:
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

				// The distance we work with are [0, maxDistance] where maxDistance is the largest value that can be found at the extremes
				// of the display in relation to the chosen wave origin point. Ie, with (10,10) it's 320 - 10 = 310
				// To add the time component (the sin inside the sin) we pre-compute a sin table that contains values from 0 to maxDistance
				// Then we add the pixel's distance to the wave origin [0, maxDistance] and the sin [0, maxDistance] together to get [0, 2*maxDistance]
				// Now, assuming we have a look-up table of size maxDistance we can look up the sine values for each distance

				// assuming there are 16 different color channels in use, we can split each of the  maxDistance values
				// to different 16 register values.
				// 310 / 16 == 19.375
				// So a distance of 19 pixels changes the color register
				// Let's say at the center of the screen red channel is 0 and at the edges it is 7
				// All that's left is pre-generating a table of these color register values and look it up

				// Interference between two wave functions can be calculated by computing individual waves
				// and then adding, subtracting, multiplying or dividing the results
				// This needs to be noted when generating the color register look-up table though, to make sure
				// all possible distance values fit in the table

				pixel_x = x * 8;
				distance_x_a = abs(wave_origin_a_x - pixel_x) + sin_time;
				distance_x_b = abs(wave_origin_b_x - pixel_x) + sin_time; 

				distance_a = silly_sqrt((distance_x_a*distance_x_a) + (distance_y_a*distance_y_a));
				distance_b = silly_sqrt((distance_x_b*distance_x_b) + (distance_y_b*distance_y_b));
				distance = distance_a * distance_b / 1000;

				channel_index = SIN_WAVE_DATA[distance];
				arrIndex = (y * screenTileWidth + x) * rowsInTile;
				if (channel_index > 16)
					channel_index = 16;

				u32 all_chans = 0;
				all_chans += (channel_index << 28)
					+ (channel_index << 24)
					+ (channel_index << 20)
					+ (channel_index << 16)
					+ (channel_index << 12)
					+ (channel_index << 8)
					+ (channel_index << 4)
					+ (channel_index);

				for (u32 p = 0; p < 8; p++)
					tilebuffer[arrIndex + p] = all_chans;
			}
		}

		// Tile 0 is backgroud, so start indexing at 1
		u16 tileIndex = line_to_draw + 1;
		for (u32 y = line_to_draw; y < line_to_draw + lines_per_loop; y++)
		{
			for (u32 x = 0; x < screenTileWidth; x++)
			{
				arrIndex = (y * screenTileWidth + x) * rowsInTile;
				VDP_loadTileData((const u32*)&tilebuffer[arrIndex], tileIndex, 1, 0);
				VDP_fillTileMapRect(PLAN_A, tileIndex, x, y, 1, 1);
				tileIndex++;
			}
		}

		line_to_draw += lines_per_loop;
		if (line_to_draw >= screenTileHeight)
			line_to_draw = 0;

		VDP_waitVSync();
	}

	return (0);
}

