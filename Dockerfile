FROM emscripten/emsdk:3.1.31 AS base

ENV DEBIAN_FRONTEND=noninteractive

# cache apt-get update results
RUN apt-get update

COPY vfb-wrapper/ ./vfb-wrapper/
COPY renderer.cpp renderer.h ./vfb-wrapper/
WORKDIR ./vfb-wrapper

RUN emcc vfb_main.cpp renderer.cpp -o vfb_main.js -sMODULARIZE -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
