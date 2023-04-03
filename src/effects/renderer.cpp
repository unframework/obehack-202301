#include <Arduino.h>
#include <FastLED.h>
#include <math.h>

#include "../effects.h"

#define SUBROWS 31

void render16x16(unsigned char *buffer) {
  const unsigned long tFract = millis() >> 4;

  // test pattern
  for (int pos = 0; pos < ROWS * COLS; pos++) {
    const long col = pos & 15;
    const long row = pos >> 4;

    // wispy smoke effect on white bg
    // with slight quadratic warp on the y-coord to compress things near the top
    // multisampling at two positions per row
    float accum = 0.0f;

    for (int i = 0; i < 2; i++) {
      const long subrow = (row << 1) + i;

      const long invrow = (SUBROWS - subrow);
      const long xLookup = col * 7000;
      const long yLookup =
          subrow * 0x1800 - invrow * invrow * 0x80 + tFract * 0x600;
      const float noiseX = (float)inoise16(xLookup, yLookup, 0) / 0x10000;

      const float dispAmountX = noiseX - 0.5f;
      const float displacedX =
          col + dispAmountX * 21.0f * fmax(0.0f, .1f + invrow * 0.06f);

      // @todo feed a vertical displacement (noiseY) into this
      const float noiseV =
          fmin(1.0f, subrow * (0.5f / SUBROWS) +
                         (float)inoise16(0x80000, yLookup, 0) / 0x10000);

      accum += fmax(0.0f,
                    1.0f - noiseV * fmax(0, fmin(1.0f, fmin(displacedX - 6.0f,
                                                            9.0f - displacedX) *
                                                           1.2f)));
    }

    accum /= 2.0f;
    const unsigned char value = 255 * accum;

    // value = displacedX > 6.5f && displacedX < 8.5f ? 255 : 0;

    buffer[pos] = value;
  }

  // flame flicker
  for (int i = 0; i < 4; i++) {
    const long pos = (15 << 4) | (i + 6);
    const long yLookup = tFract * 0x800;
    const float noiseF =
        (float)inoise16(0x180000 + i * 0x8000, yLookup, 0) / 0x10000;

    buffer[pos] = 255 * fmax(fmax(0.0f, fabs(i - 1.5f) - 1.0f),
                             fmin(0.8f, noiseF * 6.0f - 3.0f));
  }
}
