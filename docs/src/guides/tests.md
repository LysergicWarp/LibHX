# Test discs

A test is a disc: a program that leaves a report in work RAM, a row for each thing it checks.
`tools/hx-test.ps1` builds each test, runs it on Beetle Saturn and reads the report.

## Write a test

A test is a folder `tests/<group>/<name>` of the clone. The group is what it tests: `startup`,
`cartridge`. Its `main.c`:

```c
#include <stdint.h>

#include <hx/math.h>
#include <hx/test.h>

int
main(void)
{
        hx_test_equal("mul", (uint32_t)hx_mul(HX_FIX16(3), HX_FIX16(0.5)), HX_FIX16(1.5));
        hx_test_equal("sin", (uint32_t)hx_sin(HX_DEG(90)), HX_FIX14(1));
        hx_test_end();
        return 0;
}
```

- `hx_test_equal` adds a row, which passes when the two numbers are the same.
- `hx_test_end` closes the report.

Its `Makefile`:

```make
HX_SOURCES:= main.c
include ../../../build/test.mk
```

A test that needs a cartridge in the slot names the core's option for it:

```make
HX_SOURCES:= main.c
HX_TEST_CORE_OPTIONS:= beetle_saturn_cart=extram4
include ../../../build/test.mk
```

Tests of a group that share a source keep it in the group's folder, and name it as `../main.c`.

## Check the picture

A test that draws names the picture its run is to show, and the script compares a screenshot
with it:

```make
HX_SOURCES:= main.c
HX_TEST_SCREEN_WIDTH:= 320
HX_TEST_SCREEN_LINES:= 112:001f 112:7c00
HX_TEST_SCREEN_BORDER:= 0000
include ../../../build/test.mk
```

- `HX_TEST_SCREEN_LINES` is the picture from the top: 112 lines of red, then 112 of blue.
- A colour is 4 hexadecimal digits, the 15 bits VDP2 has it in: what `VDP2_RGB` gives.
- `HX_TEST_SCREEN_BORDER` is the colour around the picture.
- The screenshot is taken after the report is there, so the program keeps its picture on
  screen past `hx_test_end`.

## Run the tests

```
pwsh tools/hx-test.ps1 -Runner <folder> -RunnerImage <image> -Bios <file>
```

With no test named, every test runs. A test is named as `<group>/<name>`:

```
pwsh tools/hx-test.ps1 -Runner <folder> -RunnerImage <image> -Bios <file> startup/high-stack
```

```
pass startup/high-stack: 28 rows
1 tests passed. Results in <clone>/out
```

> [!NOTE]
> The script runs the emulator through a program that drives a libretro core without a window.
> That program is not part of the SDK. [The test flow](../in-depth/tests.md#the-runner) says what
> the script asks of one.

## The tests so far

| Test | Checks |
| --- | --- |
| `startup/high-stack` | What `main` finds, with the stack at the top of Work RAM-H |
| `startup/low-stack` | The same with the stack below the program |
| `cartridge/none` | What startup finds in the cartridge slot, and `hx_cart_require`, with no cartridge |
| `cartridge/backup` | The same with a backup memory cartridge |
| `cartridge/1mb`, `cartridge/4mb` | The same with each RAM cartridge, and that its memory holds |
| `cartridge/1mb-joined`, `cartridge/4mb-joined` | The same with `HX_CART_JOIN` set |
| `libc/memory` | `memcpy`, `memmove`, `memset` and `memcmp`, at every length up to 72 bytes and every alignment |
| `math/cxx` | The math headers from C++, linked against the archive |
| `vdp2/back-color` | VDP2's start and screen mode, and the back screen in one colour with the border in it too |
| `vdp2/back-lines` | The same with a colour for each line, and a black border |

## More detail

- API: [`<hx/test.h>`](../api/test_8h.html).
- [Scripts](../reference/scripts.md#hx-testps1): every option of `hx-test.ps1`.
- [Build settings](../reference/build.md#testmk): the settings of a test's Makefile.
- [The test flow](../in-depth/tests.md): the report's text, the two builds, the screenshot,
  the results folder.
