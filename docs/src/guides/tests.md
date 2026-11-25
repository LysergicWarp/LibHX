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
        hx_test_show();
        return 0;
}
```

- `hx_test_equal` adds a row, which passes when the two numbers are the same.
- `hx_test_end` closes the report.
- `hx_test_show` puts the rows on screen, for [a run on a console](#see-the-report-on-a-console).

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

A picture whose lines hold more than one colour goes in a file, which
`HX_TEST_SCREEN_PICTURE` names in place of `HX_TEST_SCREEN_LINES`:

```
# A red square of 8 by 8 pixels at the top left of a blue screen
8 8:001f 312:7c00
216 320:7c00
```

- A line is a count of lines of the picture, then their runs of pixels from the left.
- The pixels of a line add up to the width. The build stops on a line where they do not.

## See the report on a console

A console has no script to read work RAM. `hx_test_show`, the last call of the test above,
puts the report's rows on screen, a band of colour for each.

- A green band is a row that passes, a red one a row that fails. The first row is at the top.
- Three black lines follow every fifth band, for counting.
- The picture holds 40 rows. With more, one more band at the bottom says so: white when the
  rows without a band all pass, red when one fails.
- A program that does not reach the call shows no bands.
- The call takes VDP2 as its own. A test that checks a picture of its own does without it.

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
| `vdp2/cells` | NBG0 as tiles over the back screen: two palettes, the flips, a transparent dot, the screen's corners |
| `vdp2/planes` | NBG3's four planes in tiles of 16 by 16 dots, 256 colours and pattern names of two words, with NBG1 in front |
| `text/primary` | The text calls with the primary font: every glyph, a string, numbers, a code the font does not hold, and places past the page's edges |
| `text/own-font` | A font of the test's own, one of its sources, as cells, a string and hexadecimal digits, with pattern names of two words |
| `report/show` | `hx_test_show`'s picture: both colours, and the gap after a fifth row |
| `report/show-all` | The same with as many rows as bands |
| `report/show-more` | The same with more rows than bands, the rows without a band passing |
| `report/show-more-failed` | The same with a row without a band failing |
| `report/show-full` | The same with a full report |
| `report/show-own` | The same with the test's own rows, after `hx_test_end` |

Every test but the `vdp2`, `text` and `report` ones ends with `hx_test_show`, so its disc
shows its report on a console.

## More detail

- API: [`<hx/test.h>`](../api/test_8h.html).
- [Scripts](../reference/scripts.md#hx-testps1): every option of `hx-test.ps1`.
- [Build settings](../reference/build.md#testmk): the settings of a test's Makefile.
- [The test flow](../in-depth/tests.md): the report's text, the bands' places on screen, the
  two builds, the screenshot, the results folder.
