#include <stdint.h>
#include <emscripten.h>

// helper code

uint8_t buffer[256] = "012345";

EMSCRIPTEN_KEEPALIVE extern "C" uint8_t *getRenderBuffer() {
  return buffer;
}

// sketch code

EMSCRIPTEN_KEEPALIVE extern "C" void render16x16(uint8_t *buffer) {
  buffer[1] = 123;
}
