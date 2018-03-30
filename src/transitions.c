#include <genesis.h>
#include "transitions.h"

void
wipe_screen(void)
{
	VDP_setPaletteColor(0, 0);
	VDP_setBackgroundColor(0);
	VDP_setPlanSize(32, 32);
	for (int x = 0; x < planWidth; x += 1) {
		for (int y = 0; y < planHeight; y += 4) {
			VDP_clearTileMapRect(PLAN_WINDOW, 0, y, x, 2);
			VDP_clearTileMapRect(PLAN_A, 0, y, x, 2);
			VDP_clearTileMapRect(PLAN_B, 0, y, x, 2);

			VDP_clearTileMapRect(PLAN_WINDOW, planWidth - x, y+2, x, 2);
			VDP_clearTileMapRect(PLAN_A, planWidth - x, y+2, x, 2);
			VDP_clearTileMapRect(PLAN_B, planWidth - x, y+2, x, 2);
		}
		VDP_waitVSync();
	}
	clear_screen();
}

void
clear_screen(void)
{
	VDP_clearSprites();
	VDP_resetScreen();
	VDP_setVerticalScroll(PLAN_A, 0);
	VDP_setVerticalScroll(PLAN_B, 0);
	VDP_setHorizontalScroll(PLAN_A, 0);
	VDP_setHorizontalScroll(PLAN_B, 0);
	VDP_setHInterrupt(0);
}

void fade_to_black(void)
{
	VDP_fadeOutAll(60, 0);
	clear_screen();
}