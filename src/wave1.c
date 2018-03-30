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

u32 wave_tilebuffer[screenTileWidth * screenTileHeight * rowsInTile];

extern u16 rgbToU16(u8 r, u8 g, u8 b);

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

s16 wave_scroll = 10;

// distance to the screen center point
u16 distance_x = 0;
u16 distance_y = 0;
s16 pixel_x = 0;
s16 pixel_y = 0;
u8 channel_index = 0;
u32 arrIndex = 0;
u16 distance = 0;
// Tile 0 is backgroud, so start indexing at 1
u16 tileIndex = 1;

void
wave1_init(void)
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
    for (u32 y = 0; y < screenTileHeight; y++)
    {
        for (u32 x = 0; x < screenTileWidth; x++)
        {
            arrIndex = (y * screenTileWidth + x) * rowsInTile;
            VDP_loadTileData((const u32*)&wave_tilebuffer[arrIndex], tileIndex, 1, 0);
            tileIndex++;
        }
    }
}

void
wave1(void)
{
    static u16 counter = 0;
    static u16 sin_time = 0;

    static u16 line_to_draw = 0;
    static u16 lines_per_loop = screenTileHeight;

    counter++;
    if (counter > SIN_TIME_COUNT)
        counter = 0;

    wave_scroll += 1;
    sin_time = SIN_TIME_DATA[counter];

    for (u32 y = line_to_draw; y < line_to_draw + lines_per_loop; y++)
    {
        pixel_y = y * 8;
        distance_y = abs(screenPixelHalfY - pixel_y) + sin_time;

        for (u32 x = 0; x < screenTileWidth; x++)
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

            pixel_x = x * 8;
            distance_x = abs(screenPixelHalfX - pixel_x) + sin_time;
            distance = silly_sqrt((distance_x*distance_x) + (distance_y*distance_y));

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
                wave_tilebuffer[arrIndex + p] = all_chans;
        }
    }

    // Tile 0 is backgroud, so start indexing at 1
    u16 tileIndex = line_to_draw + 1;
    for (u32 y = line_to_draw; y < line_to_draw + lines_per_loop; y++)
    {
        for (u32 x = 0; x < screenTileWidth; x++)
        {
            arrIndex = (y * screenTileWidth + x) * rowsInTile;
            VDP_loadTileData((const u32*)&wave_tilebuffer[arrIndex], tileIndex, 1, 0);
            VDP_fillTileMapRect(PLAN_A, tileIndex, x, y, 1, 1);
            tileIndex++;
        }
    }

    line_to_draw += lines_per_loop;
    if (line_to_draw >= screenTileHeight)
        line_to_draw = 0;

    VDP_waitVSync();
}

