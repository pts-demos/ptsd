#include <genesis.h>
#include "sin_bar.h"

#define MESSAGE "Oispa kaljaa"

int main() {
  u8 ypos, xpos;
  u8 i = 17;
  VDP_setTextPalette(1);
  while (1) {
    sin_bar();
    xpos = sin_bar_data[(sin_bar_1_index+9)%128] + sizeof(MESSAGE)/2;
    ypos = sin_bar_data[sin_bar_1_index] + 1;
    VDP_drawText(MESSAGE, xpos, ypos);
    VDP_waitVSync();
  }
  return (0);
}
