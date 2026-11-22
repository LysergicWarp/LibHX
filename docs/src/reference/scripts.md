# Scripts

The PowerShell scripts under `tools/`. Both run in PowerShell 7 and need Docker.

## hx-make.ps1

Runs make in the toolchain image.

```
pwsh tools/hx-make.ps1 [-Project <dir>] [-Bios <file>] [-Image <image>] [make's arguments]
```

| Option | What | When not given |
| --- | --- | --- |
| `-Project` | The program's folder | The current one |
| `-Bios` | The BIOS file | `HX_BIOS` on the host |
| `-Image` | The toolchain image | `hx/toolchain:gcc-16.2.0-binutils-2.47` |

The three options come first, by their full names. Every argument from the first other one on
goes to make as it is.

In the container:

| Path | What |
| --- | --- |
| `/work` | The program's folder. make runs here |
| `/hx` | The LibHX clone, read-only. `HX_ROOT` names it |
| `/bios.bin` | The BIOS, read-only, when one is given. `HX_BIOS` names it |

A program's build, with eight jobs:

```
pwsh tools/hx-make.ps1 -Project ..\game -j8
```

A library's header check:

```
pwsh tools/hx-make.ps1 -C /hx/libhxmath headers
```

A library alone, into `out`:

```
pwsh tools/hx-make.ps1 -Project out -C /hx/libhxmath HX_OUT=/work
```

## hx-test.ps1

Builds each [test disc](../guides/tests.md) from nothing, runs it on Beetle Saturn and reads its
report.

```
pwsh tools/hx-test.ps1 [-Runner <folder>] [-RunnerImage <image>] [-Bios <file>]
    [-Out <folder>] [-Image <image>] [-Region na|eu|jp] [-Frames <count>] [<test> ...]
```

| Option | What | When not given |
| --- | --- | --- |
| `-Runner` | A folder that holds `beetle-runner` and `mednafen_saturn_libretro.so`, the Beetle Saturn core. Neither is part of the SDK | `HX_RUNNER` |
| `-RunnerImage` | The image the two run in | `HX_RUNNER_IMAGE` |
| `-Bios` | The BIOS file | `HX_BIOS` |
| `-Out` | Where the builds and the results go. A test's folder there is emptied first | `out`, in the clone |
| `-Image` | The toolchain image | `hx/toolchain:gcc-16.2.0-binutils-2.47` |
| `-Region` | The region the disc runs in | `na` |
| `-Frames` | The frames a disc gets to leave its report | `3600` |

A test is named as `<library>/<name>`, for the folder `<library>/test/<name>`. With none named,
every test runs.

The exit code is 0 when every test passes.

[The test flow](../in-depth/tests.md) has what the script checks, what it writes to `-Out`, and
what it asks of the runner.
