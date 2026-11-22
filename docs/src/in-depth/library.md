# Inside a library

How a library's folder is laid out and built, with libhxmath as the example.

## Layout

| Folder | What |
| --- | --- |
| `include/hx/` | The public headers. The small functions are inline in them |
| `src/` | The rest, in C and assembly (`*.sx`) |
| `ref/` | A C reference for each assembly function |
| `test/<name>/` | The library's [test discs](../guides/tests.md) |

Hot paths are SH-2 assembly behind a C API.

## Headers

libhxmath's public headers depend on `stdint.h`, `stddef.h` and the divide unit's
[register map](../guides/registers.md). C11 is the oldest C a program is written in, and the
headers are taken from C++ as they are.

`make headers` in a library's folder checks each public header alone, as C11, C23 and C++23,
with the SDK's warnings and those of strict ISO as errors. A header has to take in all it
needs. The check writes nothing:

```
pwsh tools/hx-make.ps1 -C /hx/libhxmath headers
```

## References

libhxmath's `ref/` holds a C reference for each assembly function and for the 3D dot product
(`ref_*.c`, declared in `ref/include/hx/ref.h`). A reference gives the same results as its
function. The squared lengths and the 2D dot product have none.

The references are for tests. They build into an archive of their own, `libhxmath_ref.a`, which
a program links ahead of `libhxmath.a`.

## Layout checks

The assembly is written to the layouts of the math types. The library's build checks them, so
a type changed in a header does not build until its functions follow.

## A library alone

`make HX_OUT=<folder>` in a library's folder builds its archives into that folder:

```
pwsh tools/hx-make.ps1 -Project out -C /hx/libhxmath HX_OUT=/work
```

`HX_OUT` holds no space. The same sources and toolchain give the same archives, byte for byte,
wherever the clone and `HX_OUT` are.
