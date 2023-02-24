const { spawn } = require('child_process');
const express = require('express');

const BUILD_DIR = `${__dirname}/build`;

const app = express();
app.use(express.static(BUILD_DIR));
app.post('/build', (req, res) => {
  // build the new file
  const outJSFile = `vfb${Date.now()}.js`;

  const makeProcess = spawn(
    '/bin/bash',
    [
      '-e',
      '-c',
      `OBJDIR=${BUILD_DIR} OUTPUTFILE=${outJSFile} emmake make all`,
    ]
  );

  makeProcess.stdout.pipe(process.stdout);
  makeProcess.stderr.pipe(process.stderr);

  makeProcess.on('exit', (code) => {
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

  makeProcess.on('error', (error) => {
    console.error('spawn error', error);
    res.status(500);
    res.send('');
  });
});

const server = app.listen(3000);
