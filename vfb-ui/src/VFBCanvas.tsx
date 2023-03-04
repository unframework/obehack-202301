import React, { useEffect, useRef } from 'react';

const WIDTH = 360;
const HEIGHT = 520;

const MARGIN_X = 20;
const MARGIN_Y = 30;
const PIX_W = 10;
const PIX_H = 10;

const PIX_STRIDE_X = (WIDTH - MARGIN_X * 2 - PIX_W) / 15;
const PIX_STRIDE_Y = (HEIGHT - MARGIN_Y * 2 - PIX_H) / 15;

const fills = [
  '#000000',
  '#1f1f1f', // starts a bit brighter than 1/16th
  '#2f2f2f',
  '#3f3f3f',
  '#4f4f4f',
  '#5f5f5f',
  '#6f6f6f',
  '#7f7f7f',
  '#8f8f8f',
  '#9f9f9f',
  '#afafaf',
  '#bfbfbf',
  '#cfcfcf',
  '#dfdfdf',
  '#efefef',
  '#ffffff',
];

export type VFBRenderCallback = () => Uint8Array;

export const VFBCanvas: React.FC<{ render: VFBRenderCallback }> = ({
  render,
}) => {
  // stash inside ref to avoid resetting effect
  const rendererRef = useRef(render);
  rendererRef.current = render;

  const canvasRef = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    const ctx = canvasRef.current!.getContext('2d')!;

    // opaque background
    ctx.fillStyle = '#000';
    ctx.fillRect(0, 0, WIDTH, HEIGHT);

    let currentCallbackId: number = -1;

    const rafCallback = () => {
      // schedule next
      currentCallbackId = requestAnimationFrame(rafCallback);

      ctx.fillStyle = 'rgba(0,0,0,0.25)';
      ctx.fillRect(0, 0, WIDTH, HEIGHT);

      // render and get the buffer
      const buffer = rendererRef.current(); // local var for speed
      if (buffer.length !== 256) {
        cancelAnimationFrame(currentCallbackId);
        throw new Error('expecting 256 byte render buffer');
      }

      // blit onto the canvas
      for (let i = 0; i < 256; i++) {
        const row = i >> 4;
        const col = i & 15;

        const value = buffer[i] >> 4;

        ctx.fillStyle = fills[value];
        ctx.fillRect(
          MARGIN_X + col * PIX_STRIDE_X,
          MARGIN_Y + row * PIX_STRIDE_Y,
          PIX_W,
          PIX_H
        );
      }
    };

    // kick off the RAF loop
    currentCallbackId = requestAnimationFrame(rafCallback);

    // stop loop when unmounting
    return () => {
      cancelAnimationFrame(currentCallbackId);
    };
  }, []);

  return <canvas width={WIDTH} height={HEIGHT} ref={canvasRef} />;
};
