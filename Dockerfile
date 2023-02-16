FROM emscripten/emsdk:3.1.31 AS base

ENV DEBIAN_FRONTEND=noninteractive

# cache apt-get update results
RUN apt-get update

COPY run.js *.cpp .
RUN emcc hello.cpp -o hello.js -sMODULARIZE -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
