#include <genesis.h>
#include <vdp.h>
#include "interrupt_test.h"
#include "gfx.h"

int current_scroll = 0;
int scanline_number = 0;

void h_interrupt_cb(void){
    if(scanline_number > 70){
        current_scroll -= 4;
        if(scanline_number%2 == 0){
            VDP_setHorizontalScroll(PLAN_WINDOW, 2);
        } else {
            VDP_setHorizontalScroll(PLAN_WINDOW, -2);        
        }
    }
    VDP_setVerticalScroll(PLAN_WINDOW, current_scroll);
    scanline_number++;
}

void v_interrupt_cb(void){
    VDP_setVerticalScroll(PLAN_WINDOW, 0);
    current_scroll = 0;
    scanline_number = 0;
}

void init_interrupt_test(void)
{
	SYS_disableInts();

	VDP_setScreenHeight240();
	VDP_setScreenWidth320();

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
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
	VDP_waitVSync();
	return (0);
}

int end_interrupt_test(void)
{
    SYS_disableInts();
    SPR_end();
    VDP_setHInterrupt(FALSE);
    SYS_enableInts();
    return (0);
}