#ifndef HX_VDP2_H
#define HX_VDP2_H

#include <stdint.h>

#include <hx/hw/315/vdp2.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @file
 * VDP2: its start, the screen mode, and the back screen.
 *
 *     vdp2_start();
 *     vdp2_back_color(0x7FFFE, VDP2_RGB(0, 0, 31));
 *     vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, VDP2_BORDER_BLACK);
 *
 * vdp2_start comes first: no other call counts on what the BIOS left in the chip.
 *
 * The driver takes no VRAM of its own. Where a table sits there is the program's to say, as an
 * offset into VRAM in bytes.
 *
 * The calls keep no state: each writes the chip and returns. Calls from an interrupt handler
 * or from the second SH-2 are the program's to keep apart from the main program's. */

/** A colour of 5 bits each, 0 to 31, as a word of the back screen's table holds it. More bits
 * of a value are dropped. */
#define VDP2_RGB(red, green, blue) \
        ((uint16_t)((((blue) & 31) << 10) | (((green) & 31) << 5) | ((red) & 31)))

/** The word of VRAM at offset, in bytes. The offset wraps at the VRAM's size, and its bit 0 is
 * dropped. */
static inline uint16_t *
vdp2_vram(uint32_t offset)
{
        return (uint16_t *)(uintptr_t)(VDP2_VRAM + (offset & (VDP2_VRAM_SIZE - 2)));
}

/** Puts every register a program writes to 0, the value a reset leaves there, TVMD first.
 *
 * After it:
 *   - The display is off and the screen is black.
 *   - No scroll screen and no sprite shows: with the display on, the back screen alone does.
 *   - VRAM is its two halves, A and B, neither split into banks. While no scroll screen is
 *     on, the CPU reads and writes it at any time.
 *   - The colour RAM holds 1024 colours of 5 bits each.
 *   - The back screen is one colour, the word at VRAM's offset 0.
 *
 * VRAM and the colour RAM are left as they are. */
void vdp2_start(void);

/** The screen's width in pixels, as TVMD's HRESO has it.
 *
 * 352 and 704 run on the system clock's other setting, which the boot ROM's clock change
 * routine sets. vdp2_screen leaves the clock as it is. The change puts VDP2 back as a reset
 * leaves it and does not keep VRAM, so vdp2_start and the tables come after it.
 *
 * The last four are for a 31 kHz monitor or a Hi-Vision one. They are 480 lines whatever the
 * lines value, and take VDP2_INTERLACE_NONE alone. From one of them to another width, VDP2 is
 * to be reset first. */
typedef enum {
        VDP2_WIDTH_320 = 0,
        VDP2_WIDTH_352 = 1,
        VDP2_WIDTH_640 = 2,
        VDP2_WIDTH_704 = 3,
        VDP2_WIDTH_320_31KHZ = 4,
        VDP2_WIDTH_352_HIVISION = 5,
        VDP2_WIDTH_640_31KHZ = 6,
        VDP2_WIDTH_704_HIVISION = 7
} vdp2_width_t;

/** The screen's lines, as TVMD's VRESO has them. 256 is for a PAL console alone: vdp2_pal says
 * which the console is. */
typedef enum {
        VDP2_HEIGHT_224 = 0,
        VDP2_HEIGHT_240 = 1,
        VDP2_HEIGHT_256 = 2
} vdp2_height_t;

/** The interlace, as TVMD's LSMD has it. Single shows the same lines in both fields. Double
 * shows other lines in each, for twice the lines. */
typedef enum {
        VDP2_INTERLACE_NONE = 0,
        VDP2_INTERLACE_SINGLE = 2,
        VDP2_INTERLACE_DOUBLE = 3
} vdp2_interlace_t;

/** What the border around the picture shows, as TVMD's BDCLMD has it: black, or the back
 * screen. With a colour for each line it is the colour of the picture's last line. */
typedef enum {
        VDP2_BORDER_BLACK = 0,
        VDP2_BORDER_BACK = 1
} vdp2_border_t;

/** Sets the screen mode and turns the display on: all of TVMD in one write.
 *
 * With the display on already, the write waits for the start of the next vertical blank,
 * under two frames. With the display off the chip is in blank throughout: the write waits
 * for TVSTAT to say so, and is made at once where it says so on every line.
 *
 * Each value goes into its field of TVMD as it is, cut to the field's bits. A value the
 * enumeration does not name, or values the chip does not take together, give what the chip
 * gives for those bits.
 *
 * It reads TVSTAT, which clears that register's EXLTFG and EXSYFG. Not for an interrupt
 * handler. */
void vdp2_screen(vdp2_width_t width, vdp2_height_t height, vdp2_interlace_t interlace,
    vdp2_border_t border);

/** 1 on a PAL console and 0 on an NTSC one, from TVSTAT. The read clears that register's
 * EXLTFG and EXSYFG. */
uint32_t vdp2_pal(void);

/** One colour for the whole back screen. The colour is written to VRAM at offset, and the back
 * screen's table is put there.
 *
 * The offset wraps at the VRAM's size, and its bit 0 is dropped. Bit 15 of the colour is
 * ignored by the chip. */
void vdp2_back_color(uint32_t offset, uint16_t color);

/** A colour for each line of the back screen, from the table at offset in VRAM. The table is
 * the program's to fill, through vdp2_vram: a word a line from the top, each as VDP2_RGB
 * gives it.
 *
 * With single interlace a word is two lines. With double interlace the table holds the lines
 * of both fields.
 *
 * The offset wraps at the VRAM's size, and its bit 0 is dropped. A table that runs past the
 * end of VRAM gives what the chip reads there. */
void vdp2_back_lines(uint32_t offset);

#ifdef __cplusplus
}
#endif

#endif /* !HX_VDP2_H */
