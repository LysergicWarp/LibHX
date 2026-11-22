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

## What the script does

1. Builds the test from nothing, twice. The clone and the build folder are at other places the
   second time.
2. Compares the two builds. The archives, the program and the disc image have to be the same
   bytes.
3. Runs the first build's disc until the report's mark is in work RAM, or the frames run out.
   The core gets the options the test names.
4. Reads the report.

A test passes when its report is whole and every row in it passes.

## The results

In `-Out`:

| File | What |
| --- | --- |
| `run.txt` | The clone's commit, the toolchain image and its checksum, the runner's image, the BIOS's checksum |
| `<library>/<name>/first`, `second` | The test's two builds |
| `<library>/<name>/build.log` | The builds' output |
| `<library>/<name>/result.json`, `log.txt` | The runner's output |
| `<library>/<name>/report.txt` | The report |

## The runner

The script runs the emulator through a program that drives a libretro core without a window.
That program is not part of the SDK. The script calls it as:

```
beetle-runner --core <core> --image <cue> --bios <file> --region <region> --frames <count>
    --stop-address <address> --stop-ascii <text> --capture <address>:<length> ...
    --core-option <name>=<value> ...
```

- `--core-option` sets an option of the core. It is given once for each option the test names
  in `HX_TEST_CORE_OPTIONS`: the cartridge in the slot, for one.
- It runs the disc until the text is at the address in work RAM, or for the count of frames.
  It looks for the text once a frame, after the frame has run. A test may count on that: text
  the program takes away again within the frame is not found.
- It exits with 0 when the text was there and with 2 when the frames ran out.
- It writes one JSON object to its output. Its `captures` is a list in the order asked, each
  with `data`: the bytes at that address as hexadecimal digits.
- A capture is 4096 bytes at most.
