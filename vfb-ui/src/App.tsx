import React from 'react';

import { VFBCanvas } from './VFBCanvas';

export const App = () => {
  return (
    <div className="flex flex-column justify-center items-center w-full h-full">
      <div className="flex border border-neutral-600">
        <VFBCanvas />
      </div>
    </div>
  );
};
