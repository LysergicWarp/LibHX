# Setting up

hx builds in a Docker image that holds the SH-2 toolchain. A PowerShell script runs make in it,
so the host needs no compiler of its own.

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

3. Check the setup with the math library's header check. It compiles each public header alone
   and writes nothing:

   ```
   pwsh tools/hx-make.ps1 -C /hx/libhxmath headers
   ```

Next: [Your first program](first-program.md).

## More detail

- [Scripts](../reference/scripts.md): the options of `hx-make.ps1` and what it mounts.
- [The build](../in-depth/build.md#the-toolchain-image): what the image holds.
