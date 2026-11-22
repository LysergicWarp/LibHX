# The test flow

What `tools/hx-test.ps1` does with a [test disc](../guides/tests.md), and what it asks of the
program that runs the emulator.

## The report

The report is text in work RAM, `hx_test_report`. A row is a line, and the last line counts
them:

```
pass <name> <got> <wanted>
FAIL <name> <got> <wanted>
end <rows> <failed>
```

- The numbers are 8 hexadecimal digits.
- A line `full` ahead of the last says rows were left out for want of room. They are counted
  all the same.
- A mark, `HXREPORT`, sits ahead of the text. It is written last, so a report that is there is
  whole.

## The report on screen

`hx_test_show` draws the rows with VDP2's back screen alone, a colour for each line, so it
needs no font and no scroll screen. The screen is 320 by 224 with a black border.

| Lines | What |
| --- | --- |
| 0 to 1 | Black |
| 2 to 217 | The first 40 rows: a band of 4 lines each, a black line between two, 3 black lines after every fifth |
| 218 to 221 | The band for more rows, black when there are none |
| 222 to 223 | Black |

- Row `n`, counted from 0, starts at line `2 + 5 * n + 2 * (n / 5)`, the division whole.
- The rows are read from the report's text: a line that starts with `pass` or `FAIL`. So the
  call shows the rows so far at any time, ahead of `hx_test_end` too.
- A report with the line `full` has the band for more rows in red: rows are missing from it.
- The table of colours is the first 448 bytes of VRAM.

The `report` tests write a text into the report by hand, show it, take it out again, and
then leave rows of their own. `report/show-own` shows its own rows. The script compares the
picture as it does any other.

## What the script does

1. Builds the test from nothing, twice, for the region it will run in. The clone and the
   build folder are at other places the second time.
2. Compares the two builds. The archives, the program and the disc image have to be the same
   bytes.
3. Runs the first build's disc until the report's mark is in work RAM, or the frames run out.
   The core gets the options the test names.
4. Reads the report.
5. For a test that names a picture, runs the disc again and compares a screenshot with it.

A test passes when its report is whole, every row in it passes, and the screenshot is the
picture it names.

## The screenshot

The first run ends at the frame the report is found after. The second has no stop: it runs
two frames further, and that last frame is the screenshot.

- The picture sits in the screenshot's middle. The pixels around it are the border.
- Every pixel is compared, the picture's with its line's colour and the others with the
  border's.
- A colour is compared by 5 bits each of red, green and blue, as VDP2 has it.
- A picture in its border's colour does not show its size: any screenshot that large or
  larger in the one colour passes.

## The results

In `-Out`:

| File | What |
| --- | --- |
| `run.txt` | The clone's commit, the toolchain image and its checksum, the runner's image, the BIOS's checksum |
| `<group>/<name>/first`, `second` | The test's two builds |
| `<group>/<name>/build.log` | The builds' output |
| `<group>/<name>/result.json`, `log.txt` | The runner's output |
| `<group>/<name>/report.txt` | The report |
| `<group>/<name>/screen.bmp` | The screenshot, for a test that names a picture |
| `<group>/<name>/screen.json`, `screen-log.txt` | The runner's output on the run for the screenshot |

## The runner

The script runs the emulator through a program that drives a libretro core without a window.
That program is not part of the SDK. The script calls it as:

```
beetle-runner --core <core> --image <cue> --bios <file> --region <region> --frames <count>
    --stop-address <address> --stop-ascii <text> --capture <address>:<length> ...
    --core-option <name>=<value> ...
```

and, for the screenshot, as:

```
beetle-runner --core <core> --image <cue> --bios <file> --region <region> --frames <count>
    --screenshot <frame>:<file> --core-option <name>=<value> ...
```

- `--core-option` sets an option of the core. It is given once for each option the test names
  in `HX_TEST_CORE_OPTIONS`: the cartridge in the slot, for one.
- It runs the disc until the text is at the address in work RAM, or for the count of frames.
  It looks for the text once a frame, after the frame has run. A test may count on that: text
  the program takes away again within the frame is not found.
- It exits with 0 when the text was there and with 2 when the frames ran out.
- It writes one JSON object to its output. Its `captures` is a list in the order asked, each
  with `data`: the bytes at that address as hexadecimal digits. Its `framesExecuted` is the
  count of frames it ran.
- A capture is 4096 bytes at most.
- With no text to stop on, it runs for the count of frames and exits with 0.
- `--screenshot` writes the picture of a frame, counted from 1, to a file: a BMP of 32 bits a
  pixel, the top row first. The two runs of a disc give the same frames.
