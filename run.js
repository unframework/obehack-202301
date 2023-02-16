const fc = require('./hello.js');

fc().then((m) => {
  // WASM memory is not expected to grow dynamically
  const renderBufferPtr = m.ccall('getRenderBuffer', 'number');
  const renderView = m.HEAPU8.subarray(renderBufferPtr, renderBufferPtr + 256);

  console.log('before', renderView);
  const renderer = m.cwrap('render16x16', null, ['number']);

  renderer(renderBufferPtr);
  console.log('after', renderView);
});
