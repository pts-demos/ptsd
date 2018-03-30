#include <genesis.h>
#include <timer.h>
#include <music.h>
#include <z80_ctrl.h>
#include "sin_bar.h"
#include "cryptopts.h"
#include "scroll.h"
#include "wave1.h"

int main(void)
{
	VDP_setScreenHeight240();
	PSG_init();
	SND_startPlay_VGM(sonic1);
	sin_bar_init();
	while (getTick() < 300*1)
		sin_bar();
	wave1_init();
	while (getTick() < 300*20)
		wave1();
	scroll_init();
	/* TODO: scroll() maybe writes out of bounds / Gekko; if it's before
	 * sin_bar(), sin_bar() bugs */
	while (getTick() < 300*30)
		scroll();
	/* FIXME crypto_pts never exits */
	while (getTick() < 300*90)
		crypto_pts();
	return 0;
}
