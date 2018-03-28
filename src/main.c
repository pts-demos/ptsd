#include <genesis.h>
#include "interrupt_test.h"
#include "z80_ctrl.h"

int main(void)
{
	VDP_setScreenHeight240();
	PSG_init();
	init_interrupt_test();
	while (1) {
		interrupt_test();
	}
	return (0);
}
