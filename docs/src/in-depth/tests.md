# The test flow

What `tools/hx-test.ps1` does with a [test disc](../guides/tests.md), and what it asks of the
program that runs the emulator.

## The report

The report is text in work RAM, `hx_test_report`. A row is a line, a note is a line, and the
last line counts the rows:

```
pass <name> <got> <wanted>
FAIL <name> <got> <wanted>
note <name> <value>
end <rows> <failed>
```

- The numbers are 8 hexadecimal digits.
- A note is a value to read. It is no row, and the last line does not count it.
- A line `full` ahead of the last says rows or notes were left out for want of room. The rows
  are counted all the same.
- A mark, `HXREPORT`, sits ahead of the text. It is written last, so a report that is there is
  whole.

## The report on screen

`hx_test_show` writes the report as text on NBG0, in the primary font: 40 columns by 28
lines on a screen of 320 by 224, black behind the text and around the picture.

| Line | What |
| --- | --- |
| 0 | `rows <count>, failed <count>` in decimal, then `, report full` for a full report |
| 1 to 27 | A line of the report each: the rows that fail, then the notes, then the rows that pass |
| 27 | With more than 27 lines of the report: `<count> more not shown`, and lines 1 to 26 hold the first 26 |

| Columns | What |
| --- | --- |
| 0 to 20 | The name, its first 21 characters |
| 22 to 29 | What the row got, or the note's value |
| 31 to 38 | What was wanted, for a row that fails |

- A row that fails is red, a note white, a row that passes green. Each kind keeps the
  report's order.
- The first line counts the rows the text holds. For a full report that is fewer than the
  report's last line counts: the rows left out are not among them.
- The first line is green when every row passes, and red when one fails or the report is
  full.
- The count of lines not shown is red when a row that fails is among them, and white
  otherwise.
- The lines are read from the report's text: one that starts with `pass`, `FAIL` or `note`.
  So the call shows the report so far at any time, ahead of `hx_test_end` too.
- In VRAM: the page of pattern names from the start, the back screen's colour at `0x2000`,
  the font's glyphs as cells from `0x4000`. The inks are colours 1, 17 and 33.
- The call writes VRAM with the display off, and turns it on last.
- A primary font that is not 8 by 8 dots gives no cell. NBG0 is then left off, and the screen
  is black.

`hx_test_bands` draws the rows with VDP2's back screen alone, a colour for each line, so it
needs no font and no scroll screen. The screen is 320 by 224 with a black border.

| Lines | What |
| --- | --- |
| 0 to 1 | Black |
| 2 to 217 | The first 40 rows: a band of 4 lines each, a black line between two, 3 black lines after every fifth |
| 218 to 221 | The band for more rows, black when there are none |
| 222 to 223 | Black |

- Row `n`, counted from 0, starts at line `2 + 5 * n + 2 * (n / 5)`, the division whole.
- A note has no band.
- A report with the line `full` has the band for more rows in red: lines are missing from it.
- The table of colours is the first 448 bytes of VRAM.

The `report` tests write a text into the report by hand, show it, take it out again, and
then leave rows of their own. `report/text-own` and `report/bands-own` show their own
report. The script compares the picture as it does any other.

## What the script does

1. Builds the test from nothing, twice, for the region it will run in. The clone and the
   build folder are at other places the second time.
2. Compares the two builds. The archives, the program and the disc image have to be the same
   bytes.
3. Runs the first build's disc until the report's mark is in work RAM, or the frames run out.
   The core gets the options the test names, and the pads the buttons it presses.
4. Reads the report.
5. For a test that names a picture, runs the disc again and compares a screenshot with it.

A test passes when its report is whole, every row in it passes, and the screenshot is the
picture it names.

## The screenshot

The first run ends at the frame the report is found after. The second has no stop: it runs
two frames further, and that last frame is the screenshot.

- The picture sits in the screenshot's middle. The pixels around it are the border.
- Every pixel is compared, the picture's with the colour the test names for it and the
  others with the border's.
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
    --core-option <name>=<value> ... --input <frame>:<port>:<button>:<held> ...
```

and, for the screenshot, as:

```
beetle-runner --core <core> --image <cue> --bios <file> --region <region> --frames <count>
    --screenshot <frame>:<file> --core-option <name>=<value> ...
    --input <frame>:<port>:<button>:<held> ...
```

- `--core-option` sets an option of the core. It is given once for each option the test names
  in `HX_TEST_CORE_OPTIONS`: the cartridge in the slot, for one.
- `--input` holds a button of a pad from a frame on, or lets it go. It is given once for each
  change the test names in `HX_TEST_INPUTS`. The frame counts from 1 and the port from 0. The
  button is the number of a libretro joypad's. The last number is 1 for held and 0 for let
  go.
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
