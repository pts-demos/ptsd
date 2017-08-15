#include <genesis.h>
#include <tools.h>

int main() {
  VDP_drawText("Oispa kaljaa", 2, 2);
  while (1) {
    VDP_waitVSync();
  }
  return (0);
}