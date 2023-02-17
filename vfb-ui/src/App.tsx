import React from 'react';

import { VFBCanvas } from './VFBCanvas';

const dummyBuffer = new Uint8Array(256);
dummyBuffer[2] = 123;

export const App = () => {
  return (
    <div className="flex flex-column justify-center items-center w-full h-full">
      <div className="flex border border-neutral-600">
        <VFBCanvas buffer={dummyBuffer} />
      </div>
    </div>
  );
};
