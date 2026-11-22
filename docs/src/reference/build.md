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

And the [switches](switches.md): `HX_STACK` and `HX_STACK_SIZE`.

## libs.mk

Builds the SDK's libraries alone, for a program on another runtime. `program.mk` includes it.

The program sets `HX_ROOT` and `HX_OUT`, and may set `HX_CC` and `HX_AR`. It gets:

| Name | What |
| --- | --- |
| `HX_CPPFLAGS` | The include paths of the SDK's headers |
| `HX_ARCHIVES` | The archives, for its link to depend on |
| `HX_LDLIBS` | The archives as the linker takes them, in link order |
| `HX_REF_CPPFLAGS`, `HX_REF_ARCHIVES`, `HX_REF_LDLIBS` | The same for libhxmath's references, each ahead of its counterpart above |
| `HX_TEST_CPPFLAGS`, `HX_TEST_ARCHIVES` | The same for a test disc's report |

## test.mk

A [test disc](../guides/tests.md): `program.mk` and `disc.mk` with their settings filled in. A
test's Makefile sets `HX_SOURCES` and includes it by its path from the test's folder.

It may set `HX_STACK`, `HX_STACK_SIZE` and `HX_IP_FIRST_READ`, and:

| Setting | What |
| --- | --- |
| `HX_TEST_CFLAGS` | More for its C and its C++, as `-D` |

make takes:

| Setting | What |
| --- | --- |
| `HX_OUT` | The folder of the build. `build`, in the test's folder, when it is not given |
| `HX_BIOS` | The BIOS file |

The build gives, by the test's folder name:

| File | What |
| --- | --- |
| `HX_OUT/<folder>.cue` | The disc |
| `HX_OUT/<folder>.elf` | The program |
| `HX_OUT/<folder>.nm` | The program's symbols, each with its address and size |

A test is C23 or C++23 at `-O2` with debug data and the SDK's warnings.
