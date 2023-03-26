#include <Arduino.h>
#include <FastLED.h>
#include <math.h>

#include "../effects.h"

void render16x16(unsigned char *buffer) {
  const unsigned long tFract = millis() >> 4;

  // test pattern
  for (int pos = 0; pos < ROWS * COLS; pos++) {
    const long col = pos & 15;
    const long row = pos >> 4;

    // wispy smoke effect on white bg
    const float noiseX =
        (float)inoise16(col * 7000, row * 0x3000 + tFract * 0x800, 0) / 0x10000;
    const float dispAmountX = noiseX - 0.5f;
    const float displacedX = col + dispAmountX * 21.0f * (.4f + (ROWS - row) * 0.08f);

    const unsigned char value =
        255 *
        (1.0f - fmax(0, fmin(1.0f, fmin(displacedX - 6.0f, 9.0f - displacedX) *
                                       1.2f)));
    // value = displacedX > 6.5f && displacedX < 8.5f ? 255 : 0;

    buffer[pos] = value;
  }
}
