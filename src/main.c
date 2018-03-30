#include <genesis.h>
#include <timer.h>
#include <music.h>
#include <z80_ctrl.h>
#include "sin_bar.h"
#include "cryptopts.h"
#include "scroll.h"

int main(void)
{
	VDP_setScreenHeight240();
	PSG_init();
	SND_startPlay_VGM(sonic1);
	sin_bar_init();
	while (getTick() < 300*10)
		sin_bar();
	scroll_init();
	/* TODO: scroll() maybe writes out of bounds / Gekko; if it's before
	 * sin_bar(), sin_bar() bugs */
	while (getTick() < 300*20)
		scroll();
	/* FIXME crypto_pts never exits */
	while (getTick() < 300*30)
		crypto_pts();
	return 0;
}
