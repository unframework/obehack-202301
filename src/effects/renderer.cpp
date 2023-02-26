#include <FastLED.h>
#include <Arduino.h>

#include "../effects.h"

void render16x16(unsigned char *buffer) {
  const unsigned long tFract = millis() >> 2;

  // test pattern
  for (int pos = 0; pos < ROWS * COLS; pos++) {
    const int col = pos & 15;
    const int row = pos >> 4;

    const unsigned short value = qsub8(inoise8(col * 50, row * 1500, tFract), 16);
    buffer[pos] = value > 144 ? 255 : (value * 4) / 3; // scale by 1.25
  }
}
