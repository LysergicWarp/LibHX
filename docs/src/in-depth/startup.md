# Startup and the linker script

What happens between the BIOS and `main`, and where the link puts a program.
[Startup and memory](../guides/memory.md) is the short form.

## Startup

The BIOS loads the program to Work RAM-H and enters it at its first byte, which is startup's
(`libhxrt/src/start.sx`). Startup:

1. sets the stack
2. purges the SH-2's cache and turns it on, four ways, for instructions and data both
3. zeroes every object without an initial value
4. copies the initial values that belong in Work RAM-L to their places
5. reads the cartridge slot's ID byte, and sets a RAM cartridge up when it is one's
6. calls `main`, and stops in a loop if it returns

The vector table, GBR and the interrupt masks of the SH-2 and the SCU stay as the BIOS left
them. The table is the BIOS's own, and the SH-2's mask is open.

Startup is entered once. The initial values in Work RAM-H are the loaded file's own bytes, so a
second entry would find them as the program left them.

## Where a program goes

`libhxrt/program.ld` links a program into Work RAM-H, from the address the BIOS loads it to:
`0x06004000` unless `HX_IP_FIRST_READ` says otherwise.

| What | Where | In the loaded file |
| --- | --- | --- |
| Code and constants | Work RAM-H | Yes |
| Objects with an initial value | Work RAM-H | Yes |
| Objects without one | Work RAM-H | No |
| `HX_LOW_RAM_DATA` objects | Work RAM-L | Yes, copied over by startup |
| `HX_LOW_RAM` objects | Work RAM-L | No |

Each line that places an object in Work RAM-L gets a section of its own, so the link drops the
ones nothing uses.

Nothing is linked into the cartridge. Startup finds it when the program runs, and the program
asks what is there.

## The cartridge

The slot's ID byte is the last byte of the A-Bus's CS1 area, `0x24FFFFFF`. Startup reads it
first and keeps it.

| ID | Cartridge | What startup does |
| --- | --- | --- |
| `0x5A` | The 1 Mbyte RAM cartridge | Sets it up, and keeps its two runs |
| `0x5C` | The 4 Mbyte RAM cartridge | Sets it up, and keeps its one run |
| Another | None, or another kind | Writes nothing to the slot |

The set-up follows Sega's Extended RAM Cartridge Manual:

1. The word 1 is written to `0x257EFFFE`.
2. The ID byte is read once more. The SCU's manual has the A-Bus set register written after a
   read of the A-Bus has completed.
3. The A-Bus set register, `ASR0`, is written with `0x23301FF0`.
4. The A-Bus refresh register, `AREF`, is written with `0x13`: refresh on, 3 waits.

`ASR0` holds a half for each of two areas:

| Half | Area | Value | What |
| --- | --- | --- | --- |
| High | CS0, the cartridge's memory | `0x2330` | A precharge after a read, and 3 waits, burst and normal |
| Low | CS1, the ID byte | `0x1FF0` | As the BIOS leaves it: the external wait, and 15 waits |

`ASR1`, for the CD block's area, stays as the BIOS left it.

The memory, by the addresses through the SH-2's cache:

| Cartridge | Runs |
| --- | --- |
| 1 Mbyte | `0x02400000` and `0x02600000`, 512 kbytes each |
| 4 Mbytes | `0x02400000`, 4 Mbytes |

The same memory is past the cache at each address plus `SH2_CACHE_THROUGH`.

### One run of 1 Mbyte

With [`HX_CART_JOIN`](../reference/switches.md#hx_cart_join) as `1` and the 1 Mbyte cartridge
in, startup tests whether the first block shows again just below the second, from
`0x02580000`. Sega's manual does not say that it does.

- The test is made at the block's first long and at its last, past the cache.
- A value written at the block has to read back at the other address, and one written there
  at the block.
- Both longs hold at the end what they held before.
- Where both ends pass, the program gets one run: `0x02580000`, 1 Mbyte.
- Where one fails, it gets the two runs.

A cartridge whose block shows again at its two ends and not between them would pass the test
and not hold as one run.

## The stack below the program

With `HX_STACK` as `low`, the stack runs from the program's first byte down to `0x06001000`.

- It lies over the Initial Program, which has run by then.
- It ends where the second SH-2's stack grows down from when the disc's header names none.
- Work RAM-H below that is the BIOS's own.

A load address above the usual one makes this stack larger.

## What stops the link

- A constructor or a destructor of a static object. Startup runs none yet.
- A section the linker script does not name.
- A stack size that leaves the program no memory, or one that is no multiple of 4.
- A load address that is not between the Initial Program and the end of Work RAM-H.
