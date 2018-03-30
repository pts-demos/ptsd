#include <genesis.h>
#include "sin_bar.h"
#include "music.h"
#include "z80_ctrl.h"
#include "cryptopts.h"
#include "timer.h"

int main(void)
{
	VDP_setScreenHeight240();
	PSG_init();
	SND_startPlay_VGM(sonic1);
	sin_bar_init();
	while (getTick() < 300*10)
		sin_bar();
	/* FIXME crypto_pts never exits */
	while (getTick() < 300*30)
		crypto_pts();
	return (0);
}
