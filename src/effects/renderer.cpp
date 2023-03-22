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
        (float)inoise16(col * 4000, row * 0x1000 + tFract * 0x500, 0) / 0x10000;
    const float dispAmountX = noiseX - 0.5f;
    const float displacedX = col + dispAmountX * 21.0f * (1.2f - row * 0.0625f);

    const unsigned char value =
        255 *
        (1.0f - fmax(0, fmin(1.0f, fmin(displacedX - 6.0f, 9.0f - displacedX) *
                                       1.2f)));
    // value = displacedX > 6.5f && displacedX < 8.5f ? 255 : 0;

    buffer[pos] = value;
  }
}
