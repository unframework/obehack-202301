import React, { useEffect } from 'react';

import { VFBCanvas } from './VFBCanvas';

const CODE_SERVER_URL = '/api'; // proxied http://localhost:3000/

const dummyBuffer = new Uint8Array(256);
dummyBuffer[2] = 123;

export const App = () => {
  useEffect(() => {
    import(/* webpackIgnore: true */ `${CODE_SERVER_URL}/vfb_main.js`).then(
      (vfbModule) => {
        console.log('imported', vfbModule);
      }
    );
  }, []);

  return (
    <div className="flex flex-column justify-center items-center w-full h-full">
      <div className="flex border border-neutral-600">
        <VFBCanvas buffer={dummyBuffer} />
      </div>
    </div>
  );
};
