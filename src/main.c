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

struct effect effects[] = {
	/* render func, init func, transition, duration (sec) */
	{ wave1, wave1_init, wave1_fade, 12 },
	{ interrupt_test, init_interrupt_test, fade_to_black, 15 },
	{ wave2, wave2_init, wave2_fade, 12 },
	/* NOTE: prerendered_cube is called from scroll() and sin_bar(), so
	 * keep them together */
	{ prerendered_cube_sync, prerendered_cube_init, clear_preserve_sprites, 7 },
	{ sin_bar, sin_bar_init, clear_preserve_sprites, 20 },
	{ scroll, scroll_init, fade_to_black, 25 },
	{ crypto_pts, crypto_pts_init, clear_screen, 18 },
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
	JOY_setEventHandler(joy_cb);
	effects_init();
	while (1)
		play_effect();
}
