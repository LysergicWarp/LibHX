# Setting up

hx builds in a Docker image that holds the SH-2 toolchain: binutils, GCC, GNU make and xorriso. A
PowerShell script runs make in it, so the host needs no compiler of its own.

## What you need

- Docker
- PowerShell 7 (`pwsh`)
- Git

## Steps

1. Clone the repository:

   ```
   git clone https://github.com/LysergicWarp/LibHX.git
   cd LibHX
   ```

2. Build the toolchain image. It builds binutils and GCC from source:

   ```
   docker build --tag hx/toolchain:gcc-16.2.0-binutils-2.47 tools/image
   ```

3. Check the setup with the math library's header check. It compiles each public header alone,
   as C11, C23 and C++23, and writes nothing:

   ```
   pwsh tools/hx-make.ps1 -C /hx/libhxmath headers
   ```

## hx-make.ps1

`tools/hx-make.ps1` runs make in the image, with these folders mounted:

| Path | What |
| --- | --- |
| `/work` | The program's folder, `-Project` or the current one. make runs here. |
| `/hx` | The LibHX clone, read-only. `HX_ROOT` names it. |
| `/bios.bin` | The BIOS, read-only, when `-Bios` or `HX_BIOS` on the host names one. `HX_BIOS` names it in the container. |

The options `-Project`, `-Bios` and `-Image` come first, by their full names. Every argument from
the first other one on goes to make as it is:

```
pwsh tools/hx-make.ps1 -Project ..\game -j8
```

> [!NOTE]
> A disc build needs your own BIOS. The build cuts Sega's Security Code and the Area Code blocks
> from it, and the SDK holds no copy. The places are those of BIOS v1.01a: a file with other
> bytes there stops the build.
