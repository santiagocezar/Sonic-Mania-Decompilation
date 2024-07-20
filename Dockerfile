FROM docker.io/ps2dev/ps2dev:latest

RUN apk add cmake pkgconfig build-base

ENV PKG_CONFIG_PATH="$PS2SDK/ports/share/pkgconfig:$PS2SDK/ports/lib/pkgconfig"
ENV PATH="$PS2DEV/bin:$PS2DEV/ee/bin:$PS2DEV/iop/bin:$PS2DEV/dvp/bin:$PS2SDK/bin:$PATH"
ENV VERBOSE="1"

VOLUME /src
WORKDIR /src

COPY ps2-build.sh /bin/ps2-build

CMD ps2-build
