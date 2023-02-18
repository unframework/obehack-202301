FROM emscripten/emsdk:3.1.31 AS base

ENV DEBIAN_FRONTEND=noninteractive

# cache apt-get update results
RUN apt-get update

# cache NPM dependencies
RUN npm install --global serve

COPY vfb-wrapper/ ./vfb-wrapper/
WORKDIR ./vfb-wrapper

COPY effects.h ./
COPY effects ./effects

VOLUME ./effects

RUN emcc vfb_main.cpp effects/renderer.cpp -o vfb_main.js -sEXPORT_ES6 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap

# artifact server
EXPOSE 3000
CMD npx serve --listen 3000 --no-clipboard

# host command: docker run -it -p 3000:3000 -v ./effects:/src/vfb-wrapper/effects imagename
