# ObeHack

## Dev

Run builder:

```
docker build -t obehack ./vfb-wrapper
docker run --rm -it -p 3000:3000 -v $PWD\src:/src/sketch/src obehack
```

Run webapp:

```
(cd vfb-ui && yarn start)
```
