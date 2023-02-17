// avoid including in default Arduino build
// (only built for Emscripten virtual framebuffer viewer)
#ifdef __EMSCRIPTEN__

#include <emscripten.h>

#include "renderer.h"

// render buffer exposed from WASM to JS
unsigned char virtualBuffer[256] = "012345";

EMSCRIPTEN_KEEPALIVE extern "C" unsigned char *getVirtualBuffer() {
  return virtualBuffer;
}

EMSCRIPTEN_KEEPALIVE extern "C" void renderVirtualBuffer(unsigned char *virtualBuffer) {
  // run actual sketch renderer code (from renderer.h)
  render16x16(virtualBuffer);
}

#endif // __EMSCRIPTEN__
