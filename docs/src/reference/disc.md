# Disc settings

The settings of `build/disc.mk`, which builds a disc image around a program's 1st Read file. It
needs GNU make 4.3, the SH-2 compiler and objcopy, xorriso, and GNU dd, sha256sum, date and
truncate. The toolchain image holds them all.

No path holds a space, and no value a quote or a backslash.

The program sets:

| Setting | What |
| --- | --- |
| `HX_ROOT` | The LibHX clone |
| `HX_OUT` | A folder of its build |
| `HX_BIOS` | The BIOS file |
| `HX_DISC` | The image's path without an extension, for `.iso`, `.cue` and `.silence.bin` |
| `HX_DISC_FIRST` | The 1st Read file: the program, as the bytes the BIOS loads |
| `HX_IP_TITLE` | The Game Title, 112 characters at most |
| `HX_IP_DATE` | The Release Date, `YYYYMMDD`. The volume and its files carry it too |

It gets:

| Name | What |
| --- | --- |
| `HX_DISC_IMAGE` | The image's three files, for a goal to depend on |

## The disc's files

| Setting | What | When not set |
| --- | --- | --- |
| `HX_DISC_FILES` | A folder whose contents go on the disc beside the 1st Read file | No other file |
| `HX_DISC_VOLUME` | The volume's name: `A` to `Z`, `0` to `9` and `_`, 32 characters at most | `HX` |

> [!NOTE]
> The names in `HX_DISC_FILES` are 8.3 and upper case already. xorriso changes any other name
> and says nothing.

- A link in the folder stops the build.
- A name that would come ahead of the 1st Read file, `0` or one that begins `0.`, stops the
  build.
- With `HX_DISC_FILES` set, the image is built on every run.

## The System ID

| Setting | What | When not set |
| --- | --- | --- |
| `HX_IP_MAKER` | The Maker ID, 16 characters at most | `SEGA TP HX` |
| `HX_IP_PRODUCT` | The Product Number, 10 at most | `HX-00001` |
| `HX_IP_VERSION` | The Version | `V1.000` |
| `HX_IP_DEVICE` | The Device Information | `CD-1/1` |
| `HX_IP_AREAS` | The Compatible Area Symbols, of `J T U B K A E L` | `JTUBKAEL` |
| `HX_IP_PERIPHERALS` | The Compatible Peripherals, 16 at most | `J` |
| `HX_IP_STACK_M` | The first SH-2's stack pointer, 0 for the BIOS's own | `0` |
| `HX_IP_STACK_S` | The second SH-2's | `0` |
| `HX_IP_FIRST_READ` | The address the 1st Read file is loaded to and run from | `0x06004000` |

## The tools

| Setting | What | When not set |
| --- | --- | --- |
| `HX_CC` | The SH-2 compiler | `sh2eb-elf-gcc` |
| `HX_OBJCOPY` | The SH-2 objcopy | `sh2eb-elf-objcopy` |
