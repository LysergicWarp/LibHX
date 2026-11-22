#ifndef HX_VDP2_H
#define HX_VDP2_H

#include <stdint.h>

#include <hx/hw/315/vdp2.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @file
 * VDP2: its start, the screen mode, the back screen, and the normal scroll screens in cell
 * format.
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
 * The calls keep no state: each writes the chip and returns. Most of the chip's registers
 * cannot be read back, so a call writes its registers whole, and takes every screen that has
 * a field in them. Calls from an interrupt handler or from the second SH-2 are the program's
 * to keep apart from the main program's. */

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

/** The word of the colour RAM that holds the colour of a number, where a colour is a word of 5
 * bits each, as VDP2_RGB gives it. The number wraps at 2048.
 *
 * As vdp2_start leaves the chip, the colour RAM holds 1024 colours: a write to one of the
 * first 1024 words is made to the word 1024 further on too. */
static inline uint16_t *
vdp2_cram(uint32_t color)
{
        return (uint16_t *)(uintptr_t)(VDP2_CRAM + ((color << 1) & (VDP2_CRAM_SIZE - 2)));
}

/** Puts every register a program writes to 0, the value a reset leaves there, TVMD first. But
 * for two kinds:
 *   - The cycle patterns are VDP2_ACCESS_NONE in every timing. At 0 they are reads of NBG0's
 *     pattern names.
 *   - The coordinate increments of NBG0 and NBG1 are 1, a screen at its own size. At 0 every
 *     dot of such a screen is the first of its map.
 *
 * After it:
 *   - The display is off and the screen is black.
 *   - No scroll screen and no sprite shows: with the display on, the back screen alone does.
 *   - Every scroll screen has priority 0, at which it does not show.
 *   - VRAM is its two halves, A and B, neither split into banks. The CPU reads and writes a
 *     half at any time, until vdp2_cycle gives it to a screen.
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

/** A bank of VRAM. A half that is split is two banks of 128 kbytes. A half that is not, as
 * vdp2_start leaves both, is its bank 0 alone, and the pattern of its bank 1 is ignored. */
typedef enum {
        VDP2_BANK_A0 = 0,
        VDP2_BANK_A1 = 1,
        VDP2_BANK_B0 = 2,
        VDP2_BANK_B1 = 3
} vdp2_bank_t;

/** What VDP2 does with a bank in one timing of a cycle, as the cycle pattern registers have
 * it: a read of a screen's pattern names, of its patterns, of the vertical cell scroll table
 * of NBG0 or NBG1, a read or write by the CPU, or nothing. The chip does not allow values 8
 * to 11: they give what the chip gives. */
typedef enum {
        VDP2_ACCESS_NBG0_NAMES = 0,
        VDP2_ACCESS_NBG1_NAMES = 1,
        VDP2_ACCESS_NBG2_NAMES = 2,
        VDP2_ACCESS_NBG3_NAMES = 3,
        VDP2_ACCESS_NBG0_PATTERNS = 4,
        VDP2_ACCESS_NBG1_PATTERNS = 5,
        VDP2_ACCESS_NBG2_PATTERNS = 6,
        VDP2_ACCESS_NBG3_PATTERNS = 7,
        VDP2_ACCESS_NBG0_CELL_SCROLL = 12,
        VDP2_ACCESS_NBG1_CELL_SCROLL = 13,
        VDP2_ACCESS_CPU = 14,
        VDP2_ACCESS_NONE = 15
} vdp2_access_t;

/** A bank's cycle pattern: what the chip does in each of the eight timings, T0 to T7, each a
 * vdp2_access_t. More bits of a value are dropped. */
#define VDP2_CYCLE(t0, t1, t2, t3, t4, t5, t6, t7) \
        ((((uint32_t)(t0) & 15) << 28) | (((uint32_t)(t1) & 15) << 24) \
            | (((uint32_t)(t2) & 15) << 20) | (((uint32_t)(t3) & 15) << 16) \
            | (((uint32_t)(t4) & 15) << 12) | (((uint32_t)(t5) & 15) << 8) \
            | (((uint32_t)(t6) & 15) << 4) | ((uint32_t)(t7) & 15))

/** Sets what VDP2 does with a bank of VRAM while the screen shows. A cycle is eight timings,
 * and pattern holds them as VDP2_CYCLE gives it, T0 in the top 4 bits. At 640 or 704 pixels,
 * and in the widths for a 31 kHz or a Hi-Vision monitor, a cycle is T0 to T3 alone.
 *
 * A scroll screen that shows reads its data in the timings its banks give it, each cycle:
 *   - its pattern names once
 *   - its patterns once for 16 colours, twice for 256, four times for 2048 or 32768, eight
 *     times for 16 million
 * With the names read in T0, the patterns are read in any of T0 to T2 and T4 to T7. The
 * other pairs the chip takes are in Sega's VDP2 User's Manual, section 3.3. A screen without
 * its reads does not show as it should.
 *
 * VDP2_ACCESS_CPU lets the CPU read and write the bank's half while the screen shows: the CPU
 * waits for the timing. A half that is not split, with VDP2_ACCESS_CPU or VDP2_ACCESS_NONE
 * in every timing, is the CPU's at any time: vdp2_start leaves both so. A read of pattern
 * names or of patterns for a screen that is not shown counts as VDP2_ACCESS_CPU.
 *
 * A bank the enumeration does not name is one of the four, by its low 2 bits. */
void vdp2_cycle(vdp2_bank_t bank, uint32_t pattern);

/** The colours of a scroll screen's dots, as the character control registers have them. A dot
 * of 16, 256 or 2048 colours is the number of a colour in the colour RAM. One of 32768 or of
 * 16 million is the colour itself. NBG0 takes all five, NBG1 the first four, NBG2 and NBG3 the
 * first two. */
typedef enum {
        VDP2_COLORS_16 = 0,
        VDP2_COLORS_256 = 1,
        VDP2_COLORS_2048 = 2,
        VDP2_COLORS_32768 = 3,
        VDP2_COLORS_16M = 4
} vdp2_colors_t;

/** A character's size: one cell, 8 by 8 dots, or four cells, 16 by 16. */
typedef enum {
        VDP2_CHARACTER_8 = 0,
        VDP2_CHARACTER_16 = 1
} vdp2_character_t;

/** A pattern name's size, as the pattern name control register's top 2 bits have it.
 *
 * Two words hold all of a name: the two flips, the two special bits and a palette number of
 * 7 bits in the first, a character number of 15 bits in the second.
 *
 * One word holds bits of the palette number in its top 4 bits, the number's low 4 with 16
 * colours and its top 3 with more. Then come either the two flips and 10 bits of the
 * character number, or 12 bits of it and no flip. The bits it lacks are the screen's
 * supplement. With characters of 16 by 16, the word's bits are the character number's from
 * bit 2 up. */
typedef enum {
        VDP2_NAMES_2_WORDS = 0,
        VDP2_NAMES_1_WORD_FLIP = 2,
        VDP2_NAMES_1_WORD_WIDE = 3
} vdp2_names_t;

/** A plane's size in pages, as the plane size register has it. A page is 64 by 64 cells. */
typedef enum {
        VDP2_PLANE_1X1 = 0,
        VDP2_PLANE_2X1 = 1,
        VDP2_PLANE_2X2 = 3
} vdp2_plane_t;

/** A normal scroll screen in cell format: its map is 2 by 2 planes, a plane is pages of
 * pattern names, and a name gives a character's patterns by their place in VRAM, in steps of
 * 32 bytes. */
typedef struct {
        /** The colours of a dot */
        vdp2_colors_t colors;
        /** The size of a character */
        vdp2_character_t character;
        /** The size of a pattern name */
        vdp2_names_t names;
        /** The size of a plane */
        vdp2_plane_t plane;
        /** Where the pattern names of planes A to D start, as offsets into VRAM in bytes.
         * A is the map's top left, B its top right, C its bottom left, D its bottom right.
         *
         * A page starts on a boundary of its own size: 0x2000 bytes for 1-word names of
         * characters of 8 by 8, 0x800 for 1-word names of 16 by 16, 0x4000 and 0x1000 for
         * 2-word names. A plane of two or four pages starts on a boundary of its own size.
         * An offset between two boundaries is the one below.
         *
         * An offset wraps at the VRAM's size. The four planes sit in one run of 64 pages,
         * counted from the start of VRAM: a plane's place above its own 64 pages is taken
         * from plane A's. */
        uint32_t planes[4];
        /** What a 1-word pattern name lacks, the same for every name of the screen, as the
         * low 10 bits of the pattern name control register have it: VDP2_PNCN_SPR,
         * VDP2_PNCN_SCC, VDP2_PNCN_SPLT and VDP2_PNCN_SCN. More bits are dropped. */
        uint16_t supplement;
        /** Added to the number of every colour of the screen, in steps of 256 colours: 0 to
         * 7. More bits are dropped. With 1024 colours in the colour RAM, 4 to 7 are 0 to
         * 3. */
        uint8_t color_base;
} vdp2_nbg_t;

/** The format of every scroll screen: NBG0 to NBG3. */
typedef struct {
        vdp2_nbg_t nbg[4];
} vdp2_scroll_t;

/** Sets the format of the four normal scroll screens, each a cell format one. It writes the
 * character control, pattern name control, plane size, map and map offset registers, and
 * the colour RAM offsets of the four screens. The rotation scroll screen's fields of those
 * registers are put to 0.
 *
 * It shows no screen: vdp2_show does. A screen's pattern names, patterns and colours are
 * the program's to write, through vdp2_vram and vdp2_cram.
 *
 * Each value goes into its field as it is, cut to the field's bits: NBG1's colours to 2
 * bits, those of NBG2 and NBG3 to 1. A value the enumeration does not name, or values the
 * chip does not take together, give what the chip gives for those bits.
 *
 * format is not checked for a null pointer. */
void vdp2_scroll_format(const vdp2_scroll_t *format);

/** NBG0 for vdp2_show, as the screen display enable register has it. */
#define VDP2_NBG0 VDP2_BGON_N0ON
/** NBG1 for vdp2_show. */
#define VDP2_NBG1 VDP2_BGON_N1ON
/** NBG2 for vdp2_show. */
#define VDP2_NBG2 VDP2_BGON_N2ON
/** NBG3 for vdp2_show. */
#define VDP2_NBG3 VDP2_BGON_N3ON

/** For vdp2_show: NBG0 has no transparent dot. Without it a dot of 0 is transparent and
 * shows what is behind the screen, or with 32768 or 16 million colours a dot whose top bit
 * is 0. */
#define VDP2_NBG0_OPAQUE VDP2_BGON_N0TPON
/** The same for NBG1. */
#define VDP2_NBG1_OPAQUE VDP2_BGON_N1TPON
/** The same for NBG2. */
#define VDP2_NBG2_OPAQUE VDP2_BGON_N2TPON
/** The same for NBG3. */
#define VDP2_NBG3_OPAQUE VDP2_BGON_N3TPON

/** Shows the scroll screens named, and no other: all of the screen display enable register
 * in one write. screens is VDP2_NBG0 to VDP2_NBG3 and their _OPAQUE values, joined by |.
 * Other bits of the register are written as given, and bits past its 16 are dropped.
 *
 * A screen shows where its priority is not 0 and its banks give it its reads:
 * vdp2_nbg_priority, vdp2_cycle. */
void vdp2_show(uint32_t screens);

/** Sets the priority of NBG0 to NBG3, each 0 to 7. More bits of a value are dropped.
 *
 * A screen of priority 0 does not show. Of two screens the one with the higher number is in
 * front. With the same number NBG0 is in front of NBG1, NBG1 of NBG2, NBG2 of NBG3. */
void vdp2_nbg_priority(uint32_t nbg0, uint32_t nbg1, uint32_t nbg2, uint32_t nbg3);

#ifdef __cplusplus
}
#endif

#endif /* !HX_VDP2_H */
