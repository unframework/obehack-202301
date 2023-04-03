FROM node:16 AS base

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /app

# cache NPM dependencies
COPY package.json ./
COPY yarn.lock ./
RUN yarn

# copy source code and define main UI server runtime
COPY . .

EXPOSE 8080
CMD ["yarn", "start"]
