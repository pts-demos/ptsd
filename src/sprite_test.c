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

Sprite *sprites[10];
int sanic_pos_x[10];
int sanic_pos_y[10];
int x_direction[10];
int y_direction[10];
int sanic_speeds[10];
int current_animation[10];

void init_sprite_test(void)
{
	// disable interrupt when accessing VDP
	SYS_disableInts();

	VDP_setScreenHeight240();
	VDP_setScreenWidth320();

	// init sprites engine
	SPR_init(10, 512, 512);

	// set all palette to black
	VDP_setPaletteColors(0, (u16 *)palette_black, 64);

	// VDP process done, we can re enable interrupts
	SYS_enableInts();

	// Init sanics
    for(int i = 0; i < 10; i++){
        sprites[i] = SPR_addSprite(&sanic_sprite, 0 + i * 10, 0 + i * 10, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        sanic_pos_x[i] = i + 15 * i;
        sanic_pos_y[i] = i + 12 * i;
        if(i % 2 == 0 ){
            x_direction[i] = 1;
            y_direction[i] = 1;
        } else {
            x_direction[i] = -1;
            y_direction[i] = -1;
            SPR_setHFlip(sprites[i], TRUE);
        }
        
        sanic_speeds[i] = i + 1;
        
        current_animation[i] = i;
        if(current_animation[i] > 7) {
            current_animation[i] -= 8;
        }
        SPR_setAnim(sprites[i], current_animation[i]);
    }

	VDP_setPalette(PAL0, sanic_sprite.palette->data);
}

int sprite_test(void)
{
    for(int i = 0; i < 10; i++){
        sanic_pos_x[i] += x_direction[i] * sanic_speeds[i];
        sanic_pos_y[i] += y_direction[i] * sanic_speeds[i];
        SPR_setPosition(sprites[i], sanic_pos_x[i], sanic_pos_y[i]);
        if (sanic_pos_y[i] > screenHeight - sanic_sprite.maxNumTile || sanic_pos_y[i] < 0) {
            y_direction[i] *= -1;
            current_animation[i]++;
        }
        if (sanic_pos_x[i] > screenWidth - sanic_sprite.maxNumTile || sanic_pos_x[i] < 0) {
            x_direction[i] *= -1;
            SPR_setHFlip(sprites[i], x_direction[i] == -1);
            current_animation[i]++;
        }

        if(current_animation[i] > 7){
            current_animation[i] = 0;
        }

        SPR_setAnim(sprites[i], current_animation[i]);
    }

	SPR_update();
	VDP_waitVSync();
	return (0);
}