FROM emscripten/emsdk:3.1.31 AS base

ENV DEBIAN_FRONTEND=noninteractive

# cache apt-get update results
RUN apt-get update

# cache NPM dependencies
RUN npm install --no-save express

# clone the FastLED library with NO_MCU platform
RUN git clone \
  --single-branch \
  --branch add_host_platform \
  https://github.com/jandelgado/FastLED.git \
  ./FastLED

COPY vfb-wrapper/ ./

# test make command
# COPY src/ /src/sketch/src/
# RUN OBJDIR=./tmpdist OUTPUTFILE=test.js emmake make all

VOLUME /src/sketch/src

# artifact server
EXPOSE 3000
CMD node server.js

# host command: docker run --rm -it -p 3000:3000 -v ./src:/src/sketch/src imagename
# docker build -t obehack . ; docker run --rm -it -p 3000:3000 -v $PWD\src:/src/sketch/src obehack
