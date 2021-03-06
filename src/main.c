#include <genesis.h>
#include <timer.h>
#include <z80_ctrl.h>
#include <joy.h>
#include "music.h"
#include "sin_bar.h"
#include "cryptopts.h"
#include "scroll.h"
#include "wave1.h"
#include "wave2.h"
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

void black() {
}

void black_init() {
}

struct effect effects[] = {
	/* render func, init func, transition, duration (sec) */
	{ black, black_init, clear_screen, 250 },
	{ wave1, wave1_init, wave1_fade, 1700 },
	{ interrupt_test, init_interrupt_test, clear_screen, 1520 },
	/* NOTE: prerendered_cube is called from scroll() and sin_bar(), so
	 * keep them together */
	{ prerendered_cube_sync, prerendered_cube_init, clear_preserve_sprites, 1000 },
	{ sin_bar, sin_bar_init, clear_preserve_sprites, 2100 },
	{ scroll, scroll_init, clear_screen, 2700 },
	{ crypto_pts, crypto_pts_init, clear_screen, 1900 },
	{ black, black_init, clear_screen, 100 },
	{ end, NULL, clear_screen, 0 },
	{ NULL, NULL, NULL, 0 },
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
#if 0
	JOY_setEventHandler(joy_cb);
#endif
	effects_init();
	while (1)
		play_effect();
}
