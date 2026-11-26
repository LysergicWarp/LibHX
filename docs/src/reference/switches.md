# Switches

Every choice of speed or behaviour the SDK leaves to the program.

| Switch | What it changes | Set in | When not set |
| --- | --- | --- | --- |
| [`HX_STACK`](#hx_stack) | Where the stack is, `high` or `low` | The Makefile | `high` |
| [`HX_STACK_SIZE`](#hx_stack_size) | The size of the stack at the top of Work RAM-H | The Makefile | `0x4000` |
| [`HX_CART_JOIN`](#hx_cart_join) | The 1 Mbyte RAM cartridge's memory as one run, where the cartridge allows. Experimental | The Makefile | `0` |
| [`HX_FONT`](#hx_font) | The primary font, one of the SDK's | The Makefile | `unscii8` |
| [The level](#the-programs-level) | How the program's own code is optimised | The Makefile, in `HX_PROGRAM_CFLAGS` | No optimisation |

- A switch in the Makefile is set ahead of the include of `build/program.mk`.
- The SDK's libraries are the same archives whatever a program picks.
- No switch is a call yet.
- A function's `_round` or `_fast` form is not a switch. It is picked a call at a time, by its
  name: [Fixed-point math](../guides/math.md#names).

## HX_STACK

```make
HX_STACK:= low
```

| Value | Where | Size | What it costs |
| --- | --- | --- | --- |
| `high` | The top of Work RAM-H | `HX_STACK_SIZE` | Its size, off the memory the program has in Work RAM-H |
| `low` | Below the program | 12 kbytes at the usual load address | Nothing of the program's |

Another value stops the build.

## HX_STACK_SIZE

```make
HX_STACK_SIZE:= 0x8000
```

The size of the stack with `HX_STACK` as `high`, in bytes.

- It is a multiple of 4. Another size stops the link.
- A size that leaves the program no memory stops the link too.
- It is not set with `HX_STACK` as `low`. That stack has one size.

## HX_CART_JOIN

```make
HX_CART_JOIN:= 1
```

> [!WARNING]
> Experimental. Sega's manual gives the 1 Mbyte cartridge as two blocks and no more, so this
> holds on some cartridges and not on others.

| Value | The 1 Mbyte RAM cartridge's memory |
| --- | --- |
| `0` | Two runs of 512 kbytes |
| `1` | One run of 1 Mbyte where startup finds the first block again just below the second, and the two runs where it does not |

- A program that sets it still handles two runs.
- It changes nothing with the 4 Mbyte cartridge, or with none.
- Another value stops the build.

## HX_FONT

```make
HX_FONT:= unscii8
```

The primary font, `hx_font`: the font a text call takes when it is given none.

| Value | Font |
| --- | --- |
| `unscii8` | Unscii 8 by 8: 96 glyphs of 8 by 8 dots, for the codes 0x20 to 0x7F |

- It names one of the SDK's fonts. Another value stops the build.
- A font of the program's own is one of its sources, given to the calls that take a font. It
  takes no setting and does not replace the primary font.
- A program that defines `hx_font` itself does not link.

## The program's level

```make
HX_PROGRAM_CFLAGS:= -std=c23 -Os
```

The optimisation level is the program's pick, for its own code. `HX_PROGRAM_CXXFLAGS` holds the
one for its C++.

- A program that names none is built without optimisation.
- The functions the SDK's headers hold inline are compiled with the program, at its level.
- The SDK's libraries are built at `-O2` whatever the program picks.

## More detail

- [Startup and memory](../guides/memory.md#the-stack): the stack's place, with its bounds in C.
- [The cartridge](../guides/cartridge.md#one-run-of-1-mbyte): what a program finds with
  `HX_CART_JOIN` set.
- [Text](../guides/text.md#the-primary-font): the primary font, and a font of the program's
  own.
- [Startup and the linker script](../in-depth/startup.md#the-stack-below-the-program): what
  lies under the low stack.
- [The build](../in-depth/build.md#what-the-sdk-fixes): the flags the SDK fixes, and why.
