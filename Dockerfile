FROM gcc:latest AS builder
COPY . /home/huy/dockerProject/dockerfile1
WORKDIR /home/huy/dockerProject/dockerfile1
RUN apt-get update \
    && apt-get install -y make \
    && apt-get install -y freeglut3-dev \
    && make

FROM alpine:latest
COPY --from=builder /home/huy/dockerProject/dockerfile1/firstEmu /home/huy/dockerProject/dockerfile1/
WORKDIR /home/huy/dockerProject/dockerfile1
CMD ["./firstEmu"]
