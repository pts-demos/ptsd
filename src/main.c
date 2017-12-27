#include <genesis.h>
#include "sin_bar.h"

int main() {
  u8 ypos;
  u8 i = 17;
  VDP_setTextPalette(1);
  while (1) {
    sin_bar();
    ypos = sin_bar_data[sin_bar_1_index];
    VDP_drawText("Oispa kaljaa", 2, ypos+1);
    VDP_waitVSync();
  }
  return (0);
}
