#include <genesis.h>
#include "sin_bar.h"
#include "music.h"
#include "z80_ctrl.h"

int main(void)
{
	VDP_setTextPalette(1);
	PSG_init();
	SND_startPlay_VGM(sonic1);
	sin_bar_init();
	while (1) {
		sin_bar();
	}
	return (0);
}
