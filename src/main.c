#include <genesis.h>
#include "sin_bar.h"

int main(void)
{
	VDP_setTextPalette(1);
	while (1) {
		sin_bar();
	}
	return (0);
}
