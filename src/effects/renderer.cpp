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
    // with slight quadratic warp on the y-coord to compress things near the top
    // @todo multisampling?
    const long invrow = (ROWS - row);
    const long xLookup = col * 7000;
    const long yLookup =
        row * 0x3000 - invrow * invrow * 0x200 + tFract * 0x600;
    const float noiseX = (float)inoise16(xLookup, yLookup, 0) / 0x10000;

    const float dispAmountX = noiseX - 0.5f;
    const float displacedX = col + dispAmountX * 21.0f * (.2f + invrow * 0.1f);

    // @todo feed a vertical displacement (noiseY) into this
    const float noiseV =
        fmin(1.0f, row * (0.4f / ROWS) +
                       (float)inoise16(0x80000, yLookup, 0) / 0x10000);

    const unsigned char value =
        255 * (1.0f - noiseV * fmax(0, fmin(1.0f, fmin(displacedX - 6.0f,
                                                       9.0f - displacedX) *
                                                      1.2f)));
    // value = displacedX > 6.5f && displacedX < 8.5f ? 255 : 0;

    buffer[pos] = value;
  }
}
