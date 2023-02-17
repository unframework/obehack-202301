import React, { useEffect, useRef } from 'react';

export const VFBCanvas = () => {
  const canvasRef = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    const ctx = canvasRef.current!.getContext('2d')!;

    let currentCallbackId: number = -1;

    const rafCallback = () => {
      // schedule next
      currentCallbackId = requestAnimationFrame(rafCallback);

      ctx.fillStyle = '#888';
      ctx.rect(10, 10, 10, 10);
      ctx.fill();
    };

    // kick off the RAF loop
    currentCallbackId = requestAnimationFrame(rafCallback);

    // stop loop when unmounting
    return () => {
      cancelAnimationFrame(currentCallbackId);
    };
  }, []);

  return <canvas width="360" height="520" ref={canvasRef} />;
};
