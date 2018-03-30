#include <genesis.h>
#include <timer.h>
#include <music.h>
#include <z80_ctrl.h>
#include <joy.h>
#include "sin_bar.h"
#include "cryptopts.h"
#include "scroll.h"
#include "wave1.h"
#include "effectswitcher.h"

struct effect effects[] = {
	{ sin_bar, sin_bar_init, 8 },
	{ wave1, wave1_init, 12 },
	/* TODO: scroll() maybe writes out of bounds / Gekko; if it's before
	 * sin_bar(), sin_bar() bugs */
	{ scroll, scroll_init, 15 },
	{ crypto_pts, crypto_pts_init, 0 },
	{ NULL, NULL, 0 },
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
	SND_startPlay_VGM(sonic1);
	JOY_setEventHandler(joy_cb);
	effects_init();
	while (1)
		play_effect();
}
