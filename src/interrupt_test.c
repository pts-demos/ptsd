#include <genesis.h>
#include <vdp.h>
#include "interrupt_test.h"
#include "sprite.h"
#include "gfx.h"

int current_scroll = 0;
int scanline_number = 0;

void h_interrupt_cb(void){
    if(scanline_number > 60){
        current_scroll--;
    }
    /*if(scanline_number%2 == 0){
        VDP_setHorizontalScrollLine(PLAN_WINDOW, scanline_number, 5, 1, TRUE);
    }*/
    scanline_number++;
    VDP_setVerticalScroll(PLAN_WINDOW, current_scroll);
    
}

void v_interrupt_cb(void){
    VDP_setVerticalScroll(PLAN_WINDOW, 0);
    current_scroll = 0;
    scanline_number = 0;
}

void init_interrupt_test(void)
{
	// disable interrupt when accessing VDP
	SYS_disableInts();

	VDP_setScreenHeight240();
	VDP_setScreenWidth320();

    VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);
    SYS_setHIntCallback(h_interrupt_cb);
    SYS_setVIntCallback(v_interrupt_cb);
    VDP_setHInterrupt(TRUE);

    int ind = TILE_USERINDEX;
    VDP_drawImageEx(PLAN_A, &bga_image, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bga_image.tileset->numTile;
    VDP_setPalette(PAL1, bga_image.palette->data);

    SYS_enableInts();
}

int interrupt_test(void)
{
	//SPR_update();
	VDP_waitVSync();
	return (0);
}

int end_interrupt_test(void)
{
    SYS_disableInts();
    SPR_end();
    SYS_enableInts();
    return (0);
}