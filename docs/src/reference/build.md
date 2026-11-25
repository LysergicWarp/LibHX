# Build settings

The settings of the make fragments under `build/`. A program's Makefile sets them ahead of the
include. They need GNU make 4.3.

No path holds a space, and no value a quote or a backslash.

## program.mk

Builds a program against the SDK: its objects, the SDK's libraries, the link, and the file the
BIOS loads.

The program sets:

| Setting | What |
| --- | --- |
| `HX_ROOT` | The LibHX clone |
| `HX_OUT` | A folder of its build |
| `HX_PROGRAM` | The program's path without an extension, for `.elf`, `.map` and `.bin` |
| `HX_SOURCES` | Its sources, `.c`, `.cpp` and `.sx`, by their paths from its Makefile's folder |

It may set:

| Setting | What |
| --- | --- |
| `HX_PROGRAM_CFLAGS` | What its C is compiled with: the level, the standard, warnings, its own include paths |
| `HX_PROGRAM_CXXFLAGS` | The same for its C++ |
| `HX_PROGRAM_ASFLAGS` | The same for its assembly |
| `HX_PROGRAM_ARCHIVES` | Archives of its own, linked ahead of the SDK's |
| `HX_IP_FIRST_READ` | The address the BIOS loads the program to, as `disc.mk` has it. `0x06004000` when it is not set |
| `HX_CC`, `HX_CXX`, `HX_AR`, `HX_OBJCOPY` | The SH-2 tools, when they are not the ones on the path (`sh2eb-elf-gcc` and the rest) |

And the [switches](switches.md): `HX_STACK`, `HX_STACK_SIZE`, `HX_CART_JOIN` and `HX_FONT`.

## libs.mk

Builds the SDK's libraries alone, for a program on another runtime. `program.mk` includes it.

The program sets `HX_ROOT` and `HX_OUT`, and may set `HX_CC`, `HX_AR` and
[`HX_FONT`](switches.md#hx_font). It gets:

| Name | What |
| --- | --- |
| `HX_CPPFLAGS` | The include paths of the SDK's headers |
| `HX_ARCHIVES` | The archives and the primary font's object, for its link to depend on |
| `HX_LDLIBS` | The same as the linker takes them, in link order |
| `HX_REF_CPPFLAGS`, `HX_REF_ARCHIVES`, `HX_REF_LDLIBS` | The same for libhxmath's references, each ahead of its counterpart above |
| `HX_TEST_CPPFLAGS`, `HX_TEST_ARCHIVES` | The same for a test disc's report |

## test.mk

A [test disc](../guides/tests.md): `program.mk` and `disc.mk` with their settings filled in. A
test's Makefile sets `HX_SOURCES` and includes it by its path from the test's folder.

It may set `HX_STACK`, `HX_STACK_SIZE`, `HX_CART_JOIN`, `HX_FONT` and `HX_IP_FIRST_READ`, and:

| Setting | What |
| --- | --- |
| `HX_TEST_CFLAGS` | More for its C and its C++, as `-D` |
| `HX_TEST_CORE_OPTIONS` | The options of the emulator's core its run needs, each as `<name>=<value>`: the cartridge in the slot, for one |
| `HX_TEST_SCREEN_LINES` | The picture its run is to show, from the top: each `<lines>:<colour>` is that many lines of one colour. A colour is 4 hexadecimal digits, the 15 bits VDP2 has it in |
| `HX_TEST_SCREEN_PICTURE` | In place of `HX_TEST_SCREEN_LINES`, a file that holds the picture, for lines of more than one colour. A line of it is `<lines> <pixels>:<colour> ...`: that many lines of the picture, each those runs of pixels from the left, which add up to the width. A number is 1 to 9999. A `#` starts a comment |
| `HX_TEST_SCREEN_WIDTH` | The picture's width in pixels. Set with either |
| `HX_TEST_SCREEN_BORDER` | The colour around the picture. Set with either |

make takes:

| Setting | What |
| --- | --- |
| `HX_OUT` | The folder of the build. `build`, in the test's folder, when it is not given |
| `HX_BIOS` | The BIOS file |
| `HX_TEST_REGION` | The region of the console the disc is run on: `na`, `eu` or `jp`. `na` when it is not given. A test's sources see `HX_TEST_PAL`, 1 for `eu` and 0 otherwise |

The build gives, by the test's folder name:

| File | What |
| --- | --- |
| `HX_OUT/<folder>.cue` | The disc |
| `HX_OUT/<folder>.elf` | The program |
| `HX_OUT/<folder>.nm` | The program's symbols, each with its address and size |
| `HX_OUT/<folder>.options` | The core's options, one a line |
| `HX_OUT/<folder>.screen` | The picture: the width and the border's colour, then lines as `HX_TEST_SCREEN_PICTURE`'s file has them. No line when the test names no picture |

A test is C23 or C++23 at `-O2` with debug data and the SDK's warnings.
