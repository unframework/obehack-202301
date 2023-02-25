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
  ./vfb-wrapper/FastLED

COPY vfb-wrapper/ ./vfb-wrapper/
WORKDIR ./vfb-wrapper

COPY effects.h ./

VOLUME ./effects

# artifact server
EXPOSE 3000
CMD node server.js

# host command: docker run -it -p 3000:3000 -v ./effects:/src/vfb-wrapper/effects imagename
# docker build -t emtest2 . ; docker run --name emtest2 -it -p 3000:3000 -v ./effects:/src/vfb-wrapper/effects emtest2
