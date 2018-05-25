#include <genesis.h>
#include <vdp.h>
#include "prerendered_cube.h"
#include "sprite.h"

int cube_pos_x;
int cube_pos_y;
int cube_current_animation;
Sprite* cube_sprite;

void prerendered_cube_init(void) {
    SYS_disableInts();
	SPR_init(1, 768, 768);

	// Set all palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);
	SYS_enableInts();

	cube_sprite = SPR_addSprite(&default_cube_sprite, 0, 0,
		TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	cube_pos_x = 60;
	cube_pos_y = 40;

	cube_current_animation = 0;
	SPR_setAnim(cube_sprite, cube_current_animation);

	VDP_setPalette(PAL0, default_cube_sprite.palette->data);
}

void prerendered_cube_uninit(void) {
	SPR_setPosition(cube_sprite, 400, 300);
	VDP_waitVSync();
	SPR_update();
}

void prerendered_cube(void) {
	SPR_setPosition(cube_sprite, cube_pos_x, cube_pos_y);
	SPR_setAnim(cube_sprite, cube_current_animation);
	if (cube_sprite->seqInd > 14) {
		cube_current_animation++;
		if (cube_current_animation > 5)
			cube_current_animation = 0;
	}
	SPR_update();
	VDP_waitVSync();
}

