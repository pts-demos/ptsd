#include <genesis.h>
#include "sin_bar.h"
#include "music.h"
#include "z80_ctrl.h"

int main(void)
{
	VDP_setTextPalette(1);
	PSG_init();
	XGM_setLoopNumber(-1);
	XGM_startPlay(sonic1);
	while (1) {
		sin_bar();
	}
	return (0);
}
