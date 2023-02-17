const fc = require('./vfb_main.js');

fc().then((m) => {
  // WASM memory is not expected to grow dynamically
  const renderBufferPtr = m.ccall('getVirtualBuffer', 'number');
  const renderView = m.HEAPU8.subarray(renderBufferPtr, renderBufferPtr + 256);

  console.log('before', renderView);
  const renderer = m.cwrap('renderVirtualBuffer', null, ['number']);

  renderer(renderBufferPtr);
  console.log('after', renderView);
});
