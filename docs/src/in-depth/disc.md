# The disc image

What `build/disc.mk` puts on a disc, past what [Building a disc](../start/disc.md) needs.

## The tracks

A Saturn disc has one data track and one audio track at least, each of four seconds or more.

- The data track is the ISO, filled up to four seconds.
- The audio track is four seconds of silence behind a pause of two.
- The CUE names the two files.

## The Initial Program

The disc opens with the Initial Program: the System ID, the Security Code, the Area Code blocks,
and the code that hands over to the 1st Read file. The BIOS loads it, checks the Security Code
and the area against its own, and runs it.

The build assembles it from the [System ID's settings](../reference/disc.md#the-system-id) and
the two cuts from the BIOS. A text too long for its field stops the build.

A disc carries an Area Code block for each area it runs in. `HX_IP_AREAS` names the areas.

## The Security Code

Every disc carries Sega's Security Code. The SDK holds no copy of it, or of the Area Code
blocks.

- Both are cut from the BIOS named by `HX_BIOS`, on every build.
- Each cut is checked against its checksum.
- The places are those of BIOS v1.01a. A file with other bytes there stops the build, since
  its disc would not start.

## The ISO

The data track is ISO 9660 level 1 with nothing added, the Initial Program in its system area.

The BIOS loads the first entry of the disc's root, file or folder, in the order of the names.
The 1st Read file is `0.BIN` there. A name in `HX_DISC_FILES` that would come ahead of it
stops the build.

## The same image from the same files

Every date on the disc is the Release Date, `HX_IP_DATE`. The same files give the same image,
byte for byte.
