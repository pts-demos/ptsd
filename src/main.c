#include <genesis.h>
#include <timer.h>
#include <z80_ctrl.h>
#include <joy.h>
#include "music.h"
#include "sin_bar.h"
#include "cryptopts.h"
#include "scroll.h"
#include "wave1.h"
#include "flying_rectangles.h"
#include "effectswitcher.h"
#include "transitions.h"
#include "interrupt_test.h"
#include "prerendered_cube.h"

void
end(void)
{
	SND_stopPlay_VGM();
	VDP_resetScreen();
}

struct effect effects[] = {
	/* render func, init func, transition, duration (sec) */
	{ sin_bar, sin_bar_init, fade_to_black, 30, NULL},
	{ wave1, wave1_init, fade_to_black, 12, NULL },
	{ interrupt_test, init_interrupt_test, fade_to_black, 15, NULL},
	{ prerendered_cube, prerendered_cube_init, fade_to_black, 12, NULL},
	/* TODO: scroll() maybe writes out of bounds / Gekko; if it's before
	 * sin_bar(), sin_bar() bugs */
	{ scroll, scroll_init, fade_to_black, 25, NULL},
	{ flying_rectangles, flying_rectangles_init, fade_to_black, 15, NULL},
	{ crypto_pts, crypto_pts_init, clear_screen, 18, NULL },
	{ end, NULL, clear_screen, 0, NULL },
	{ NULL, NULL, NULL, 0, NULL },
};

void
joy_cb(u16 joy, u16 changed, u16 state)
{
	if (state & BUTTON_LEFT)
		prev_effect();
	else if (state & BUTTON_RIGHT)
		next_effect();
}

int
main(void)
{
	VDP_setScreenHeight240();
	PSG_init();
	JOY_init();
	SND_startPlay_VGM(pts_1989);
	JOY_setEventHandler(joy_cb);
	effects_init();
	while (1)
		play_effect();
}
