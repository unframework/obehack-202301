#include "renderer.h"

void render16x16(unsigned char *buffer) {
  // test pattern
  for (int pos = 0; pos < ROWS * COLS; pos++) {
    const int col = pos & 15;
    const int row = pos >> 4;

    const unsigned char gradient = (col >> 1); // reduce to a 3-bit value
    const unsigned char value =
        (col + row) & 1 ? (row & 1 ? 7 - gradient : gradient) : 0;

    buffer[pos] = value;
  }
}
