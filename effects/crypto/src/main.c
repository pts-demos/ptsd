#include <genesis.h>
#include "cryptopts.h"

int main(void)
{
	VDP_setTextPalette(1);
	while (1) {
		crypto_pts();
	}
	return (0);
}
