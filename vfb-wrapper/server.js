const { exec, execSync } = require('child_process');
const express = require('express');

const BUILD_DIR = `${__dirname}/build`;

// throw if dir already exists
execSync(`mkdir ${BUILD_DIR}`);

const app = express();
app.use(express.static(BUILD_DIR));
app.post('/build', (req, res) => {
  // clean first
  exec(`rm -f "${BUILD_DIR}/*"`, (error) => {
    if (error) {
      console.error(error);
      res.status(500);
      res.send('');
      return;
    }

    // build the new file
    const outJSFile = `vfb${Date.now()}.js`;

    exec(`emcc vfb_main.cpp effects/renderer.cpp -o build/${outJSFile} -sEXPORT_ES6 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap`, () => {
      if (error) {
        console.error(error);
        res.status(500);
        res.send('');
        return;
      }

      // report the generated file name
      res.status(200);
      res.send(`/${outJSFile}`);
    });
  });
})

const server = app.listen(3000);
