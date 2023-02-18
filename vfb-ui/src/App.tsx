import React, { useEffect } from 'react';

import { VFBCanvas } from './VFBCanvas';

const CODE_SERVER_URL = '/api'; // proxied http://localhost:3000/

const dummyBuffer = new Uint8Array(256);
dummyBuffer[2] = 123;

interface VFBRendererModule extends EmscriptenModule {
  ccall: typeof ccall;
  cwrap: typeof cwrap;
}

export const App = () => {
  useEffect(() => {
    // wait before importing, in case this gets unmounted early
    let debounceTimeoutId = -1;
    const debouncePromise = new Promise<void>((resolve) => {
      debounceTimeoutId = window.setTimeout(resolve, 300);
    });

    debouncePromise.then(async () => {
      // proceed to import module code and initialize it
      const vfbModule = await import(
        /* webpackIgnore: true */ `${CODE_SERVER_URL}/vfb_main.js`
      );
      console.log('imported module');

      const factory: () => Promise<VFBRendererModule> = vfbModule.default;
      const m = await factory();
      console.log('initialized Emscripten code', m);

      // WASM memory is not expected to grow dynamically
      const renderBufferPtr = m.ccall('getVirtualBuffer', 'number', [], []);
      const renderView = m.HEAPU8.subarray(
        renderBufferPtr,
        renderBufferPtr + 256
      );

      console.log('before', renderView);
      const renderer = m.cwrap('renderVirtualBuffer', null, ['number']);

      renderer(renderBufferPtr);
      console.log('after', renderView);
    });

    return () => {
      // prevent debounce promise from resolving after timeout
      clearTimeout(debounceTimeoutId);
    };
  }, []);

  return (
    <div className="flex flex-column justify-center items-center w-full h-full">
      <div className="flex border border-neutral-600">
        <VFBCanvas buffer={dummyBuffer} />
      </div>
    </div>
  );
};
