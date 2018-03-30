#include <genesis.h>
#include "rotating_patterns.h"
#include "music.h"
#include "z80_ctrl.h"

int main(void)
{
	VDP_setTextPalette(1);
	PSG_init();
	rotating_patterns_init();
	while (1) {
		rotating_patterns();
	}
	return (0);
}
