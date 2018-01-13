#include <genesis.h>
#include <vdp.h>
#include "sprite_test.h"
#include "sprite.h"

/**
 * Sample main:
 * 
 #include <genesis.h>
 #include "sprite_test.h"

int main(void)
{
	init_sprite_test();
	while (1) {
		sprite_test();
	}
	return (0);
}
*/

Sprite *sprites[1];
int sanic_pos_x;
int sanic_pos_y;
int x_direction = 1;
int y_direction = 1;

void init_sprite_test(void)
{
	// disable interrupt when accessing VDP
	SYS_disableInts();

	VDP_setScreenHeight240();
	VDP_setScreenWidth320();

	// init sprites engine
	SPR_init(16, 256, 256);

	// set all palette to black
	VDP_setPaletteColors(0, (u16 *)palette_black, 64);

	// VDP process done, we can re enable interrupts
	SYS_enableInts();

	// Init sanic
	sprites[0] =
		SPR_addSprite(&sanic_sprite, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	sanic_pos_x = 0;
	sanic_pos_y = 0;

	//
	VDP_setPalette(PAL0, sanic_sprite.palette->data);
	VDP_setPalette(PAL1, palette_red);
	VDP_setTextPalette(PAL1);
}

int sprite_test(void)
{
	sanic_pos_x += x_direction * 2;
	sanic_pos_y += y_direction * 2;
	SPR_setPosition(sprites[0], sanic_pos_x, sanic_pos_y);
	if (sanic_pos_y > screenHeight - sanic_sprite.maxNumTile || sanic_pos_y < 0) {
		y_direction *= -1;
	}
	if (sanic_pos_x > screenWidth - sanic_sprite.maxNumTile || sanic_pos_x < 0) {
		x_direction *= -1;
        SPR_setHFlip(sprites[0], x_direction == -1);
	}

	SPR_update();
	VDP_waitVSync();
	return (0);
}