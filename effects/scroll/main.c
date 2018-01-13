#include <genesis.h>
#include "scroll.h"
#include "music.h"
#include "z80_ctrl.h"

int main(void)
{
	VDP_setTextPalette(1);
	PSG_init();
	scroll_init();
	while (1) {
		scroll();
	}
	return (0);
}
