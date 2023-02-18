const { spawn, execSync } = require('child_process');
const express = require('express');

const BUILD_DIR = `${__dirname}/build`;

// throw if dir already exists
execSync(`mkdir ${BUILD_DIR}`);

const app = express();
app.use(express.static(BUILD_DIR));
app.post('/build', (req, res) => {
  // build the new file
  const outJSFile = `vfb${Date.now()}.js`;

  const process = spawn(
    '/bin/bash',
    [
      '-e',
      '-c',
      [
        `rm -f "${BUILD_DIR}/*"`,
        `emcc vfb_main.cpp effects/renderer.cpp -o build/${outJSFile} -sEXPORT_ES6 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap`,
      ].join('&&'),
    ],
    {
      stdio: 'pipe',
    }
  );

  process.on('exit', (code) => {
    if (code !== 0) {
      console.error('unsuccessful exit code', code);
      res.status(500);
      res.send('');
      return;
    }

    // report the generated file name
    res.status(200);
    res.send(`/${outJSFile}`);
  });

  process.on('error', (error) => {
    console.error('spawn error', error);
    res.status(500);
    res.send('');
  });
});

const server = app.listen(3000);
