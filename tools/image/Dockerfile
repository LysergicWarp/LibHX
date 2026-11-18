# syntax=docker/dockerfile:1.7@sha256:a57df69d0ea827fb7266491f2813635de6f17269be881f696fbfdf2d83dda33e
#
# The hx toolchain image: binutils and GCC for the SH-2, GNU make and xorriso. It holds nothing
# of the SDK. A program's build compiles the SDK's libraries from the LibHX clone it is given.
#
#   docker build --tag hx/toolchain:gcc-16.2.0-binutils-2.47 tools/image
#
# Every input is pinned: the Dockerfile frontend and the base image by digest, the base's
# packages by the snapshot of the day it was made, the sources by checksum.

ARG BASE_IMAGE=debian:trixie-slim@sha256:020c0d20b9880058cbe785a9db107156c3c75c2ac944a6aa7ab59f2add76a7bd
ARG SNAPSHOT=20260713T000000Z
ARG BINUTILS_VERSION=2.47
ARG BINUTILS_SHA256=154ab23b60070e8f27013c22977f1129425d67d1e8acd6e13010e617811e4cff
ARG GCC_VERSION=16.2.0
ARG GCC_SHA256=e6738e29597f733270731aa90600f37ffdc045079dfc27ec7e8192cc81085c3e

FROM ${BASE_IMAGE} AS base
ARG SNAPSHOT

# A snapshot's release files are past their date, so apt is told not to check it. That holds for
# the image too: apt-get works in it as it is.
RUN sed -i \
        -e "s|^URIs: http://deb.debian.org/debian-security$|URIs: http://snapshot.debian.org/archive/debian-security/${SNAPSHOT}|" \
        -e "s|^URIs: http://deb.debian.org/debian$|URIs: http://snapshot.debian.org/archive/debian/${SNAPSHOT}|" \
        /etc/apt/sources.list.d/debian.sources \
    && test "$(grep -c "^URIs: http://snapshot.debian.org/archive/.*/${SNAPSHOT}$" /etc/apt/sources.list.d/debian.sources)" = 2 \
    && printf 'Acquire::Check-Valid-Until "false";\n' > /etc/apt/apt.conf.d/99snapshot

FROM base AS build
ARG BINUTILS_VERSION
ARG BINUTILS_SHA256
ARG GCC_VERSION
ARG GCC_SHA256

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        gcc g++ libc6-dev make flex bison gawk xz-utils \
        libgmp-dev libmpfr-dev libmpc-dev \
    && rm -rf /var/lib/apt/lists/*

ADD --checksum=sha256:${BINUTILS_SHA256} \
    https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz /src/binutils.tar.xz
ADD --checksum=sha256:${GCC_SHA256} \
    https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz /src/gcc.tar.xz

ENV T=/opt/sh2eb-elf
ENV PATH="${T}/bin:${PATH}"

# binutils first: GCC's configure asks the assembler and the linker what they can do. Plugins are
# on for link-time optimisation. ar writes no time, owner or mode into an archive.
RUN mkdir /src/binutils /build-binutils \
    && tar -xJf /src/binutils.tar.xz -C /src/binutils --strip-components=1 \
    && cd /build-binutils \
    && /src/binutils/configure \
        --target=sh2eb-elf --prefix=$T \
        --enable-plugins --enable-lto --enable-deterministic-archives \
        --disable-nls --disable-werror --disable-gprofng --disable-gdb --disable-sim \
        --without-zstd \
    && make -j"$(nproc)" MAKEINFO=true \
    && make install-strip MAKEINFO=true \
    && cd / \
    && rm -rf /build-binutils /src/binutils /src/binutils.tar.xz

# The compiler and libgcc alone. There is no C library to build the other target libraries on.
RUN mkdir /src/gcc /build-gcc \
    && tar -xJf /src/gcc.tar.xz -C /src/gcc --strip-components=1 \
    && cd /build-gcc \
    && /src/gcc/configure \
        --target=sh2eb-elf --prefix=$T --with-local-prefix=$T/sh2eb-elf \
        --with-newlib --without-headers --with-endian=big \
        --enable-languages=c,c++ --enable-lto --enable-target-optspace --enable-tls \
        --enable-multiarch \
        --disable-threads --disable-shared --disable-nls --disable-decimal-float \
        --disable-__cxa_atexit --disable-initfini-array \
        --disable-hosted-libstdcxx --disable-libstdcxx \
        --disable-libgomp --disable-libssp --disable-libquadmath --disable-libquadmath-support \
        --disable-libatomic --disable-libvtv \
        --without-isl --without-zstd \
    && make -j"$(nproc)" all-gcc all-target-libgcc \
    && make install-strip-gcc install-target-libgcc \
    && cd / \
    && rm -rf /build-gcc /src

# ar, nm and ranlib load GCC's plugin from here on their own, so they read objects that hold
# link-time optimisation data alone. The versions are recorded as the tools report them.
RUN mkdir -p $T/lib/bfd-plugins \
    && ln -s ../../libexec/gcc/sh2eb-elf/${GCC_VERSION}/liblto_plugin.so $T/lib/bfd-plugins/ \
    && test -e $T/lib/bfd-plugins/liblto_plugin.so \
    && { sh2eb-elf-as --version | head -n 1 \
        && sh2eb-elf-ld --version | head -n 1 \
        && sh2eb-elf-gcc --version | head -n 1; } > $T/versions.txt

FROM base
ARG BINUTILS_VERSION
ARG GCC_VERSION

LABEL org.opencontainers.image.title="hx toolchain" \
      org.opencontainers.image.description="binutils ${BINUTILS_VERSION} and GCC ${GCC_VERSION} for the SH-2, GNU make and xorriso" \
      org.opencontainers.image.source="https://github.com/LysergicWarp/LibHX"

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        make xorriso libgmp10 libmpfr6 libmpc3 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/sh2eb-elf /opt/sh2eb-elf

ENV PATH="/opt/sh2eb-elf/bin:${PATH}"
WORKDIR /work
