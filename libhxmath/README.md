# libhxmath

Fixed-point math for the SH-2, in C and assembly. Its public headers depend on `stdint.h` alone.

## Layout

- `include/hx/` holds the public headers. `<hx/math.h>` takes everything in and states the
  conventions. The small functions are inline in the headers.
- `src/` holds the rest, in C and assembly (`*.sx`).
- `ref/` holds a C reference for each assembly function and for the 3D dot product
  (`ref_*.c`, declared in `ref/include/hx/ref.h`). A reference gives the same results as its
  function. The squared lengths and the 2D dot product have none. The references are for
  tests and build into an archive of their own.

## Build

`make HX_OUT=<folder>` builds `libhxmath.a` and `libhxmath_ref.a` into that folder, with
`HX_CC` and `HX_AR` as the SH-2 compiler and archiver. Nothing is written beside the sources.
A program that links the references puts `libhxmath_ref.a` ahead of `libhxmath.a`. The flags are
the SDK's, from [`build/flags.mk`](../build/flags.mk).

`make headers` checks each public header alone, as C11, C23 and C++23, with `HX_CXX` as the C++
compiler. It builds nothing.

The same sources and toolchain give the same archives, byte for byte, wherever the clone and
`HX_OUT` are. `HX_OUT` holds no space. Debug data from the compiler names the build folder as
`LibHX/libhxmath`, and source files relative to it.
