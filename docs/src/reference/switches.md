# Switches

Every choice of speed or behaviour the SDK leaves to the program.

| Switch | What it changes | Set in | When not set |
| --- | --- | --- | --- |
| [`HX_STACK`](#hx_stack) | Where the stack is, `high` or `low` | The Makefile | `high` |
| [`HX_STACK_SIZE`](#hx_stack_size) | The size of the stack at the top of Work RAM-H | The Makefile | `0x4000` |
| [The level](#the-programs-level) | How the program's own code is optimised | The Makefile, in `HX_PROGRAM_CFLAGS` | No optimisation |

- A switch in the Makefile is set ahead of the include of `build/program.mk`.
- The SDK's libraries are the same archives whatever a program picks.
- No switch is a call yet.

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
- [Startup and the linker script](../in-depth/startup.md#the-stack-below-the-program): what
  lies under the low stack.
- [The build](../in-depth/build.md#what-the-sdk-fixes): the flags the SDK fixes, and why.
