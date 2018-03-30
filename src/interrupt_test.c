#include <genesis.h>
#include <vdp.h>
#include "interrupt_test.h"
#include "gfx.h"

int current_scroll = 0;
int scanline_number = 0;
int sine_counter = 0;
int sine_factor_wave = 0;
int sine_factor_ripple = 0;
int sine_wave = 0;
int sine_ripple = 0;
int total_scroll = 0;

int sine_table[] = {
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,2
};
#define SINE_COUNT (sizeof(sine_table) / sizeof(sine_table[0]))

void h_interrupt_cb(void){
    if(scanline_number > 76 + sine_table[sine_wave]){
        current_scroll -= 7;
        VDP_setVerticalScroll(PLAN_A, total_scroll + current_scroll);
    }
    
    if(scanline_number > 76 && scanline_number%2 == 0){
        VDP_setHorizontalScroll(PLAN_A, sine_table[sine_ripple]);
    } else if(scanline_number > 76 && scanline_number%2 == 1){
        VDP_setHorizontalScroll(PLAN_A, -sine_table[sine_ripple]);        
    }
    
    scanline_number++;
    if(++sine_factor_wave > 70){
        sine_factor_wave = 0;
        if(++sine_wave >= SINE_COUNT){
            sine_wave = 0;
        } 
    }
    if(++sine_factor_ripple > 120){
        sine_factor_ripple = 0;
        if(++sine_ripple >= SINE_COUNT){
            sine_ripple = 0;
        } 
    }
}


void init_interrupt_test(void)
{
	SYS_disableInts();

	VDP_setScreenHeight240();
	VDP_setScreenWidth320();

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    SYS_setHIntCallback(h_interrupt_cb);
    VDP_setHInterrupt(TRUE);

    int ind = TILE_USERINDEX;
    VDP_drawImageEx(PLAN_A, &bga_image, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    VDP_setPalette(PAL1, bga_image.palette->data);

    SYS_enableInts();
}

int interrupt_test(void)
{
	VDP_waitVSync();
    current_scroll = 0;
    scanline_number = 0;
    VDP_setVerticalScroll(PLAN_A, total_scroll);
    VDP_setHorizontalScroll(PLAN_A, 0);
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