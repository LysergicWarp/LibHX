#include <stdint.h>

#include <hx/test.h>
#include <hx/vdp2.h>

/* VDP2's start, the screen mode and the back screen. The rows are what the registers that can
 * be read back hold. The colours are the run's screenshot to show.
 *
 * LINES gives the back screen a colour for each line, in four bands, and a black border.
 * Without it the back screen is one colour, and the border shows it too. */

/* The back screen's table: the last 4 kbytes of VRAM */
#define TABLE 0x7F000

#define SCREEN_LINES 224
#define BAND_LINES (SCREEN_LINES / 4)

#define ONE_COLOR VDP2_RGB(12, 20, 22)

#ifdef LINES
#define BORDER VDP2_BORDER_BLACK
#else
#define BORDER VDP2_BORDER_BACK
#endif

/* The V counter as it is latched, by a read of EXTEN while its EXLTEN is 0. The rows compare
 * two of these and take no line number from one */
static uint32_t
line(void)
{
        (void)VDP2->exten;
        return VDP2->vcnt;
}

int
main(void)
{
        /* Values a reset does not leave, in the four registers that can be read back */
        VDP2->tvmd = 0x8010;
        VDP2->exten = 0x0002;
        VDP2->vrsize = 0x8000;
        VDP2->ramctl = 0x1300;
        hx_test_equal("before.tvmd", VDP2->tvmd, 0x8010);
        hx_test_equal("before.exten", VDP2->exten, 0x0002);
        hx_test_equal("before.vrsize", VDP2->vrsize & 0x8000, 0x8000);
        hx_test_equal("before.ramctl", VDP2->ramctl, 0x1300);

        /* And in registers that are written alone, where the picture shows them: a colour
         * offset on the back screen, of all the red in A and all the blue in B, B selected */
        VDP2->clofen = 0x0020;
        VDP2->clofsl = 0x0020;
        VDP2->coar = 0x00FF;
        VDP2->cobb = 0x00FF;

        vdp2_start();
        hx_test_equal("start.tvmd", VDP2->tvmd, 0);
        hx_test_equal("start.exten", VDP2->exten, 0);
        hx_test_equal("start.vrsize", VDP2->vrsize & 0x8000, 0);
        hx_test_equal("start.ramctl", VDP2->ramctl, 0);

        hx_test_equal("pal", vdp2_pal(), HX_TEST_PAL);

        hx_test_equal("rgb", VDP2_RGB(1, 2, 4), 0x1041);
        hx_test_equal("rgb.cut", VDP2_RGB(32 + 1, 32 + 2, 32 + 4), 0x1041);
        hx_test_equal("vram", (uint32_t)(uintptr_t)vdp2_vram(TABLE), VDP2_VRAM + TABLE);
        hx_test_equal("vram.odd", (uint32_t)(uintptr_t)vdp2_vram(TABLE + 1), VDP2_VRAM + TABLE);
        hx_test_equal("vram.wraps", (uint32_t)(uintptr_t)vdp2_vram(VDP2_VRAM_SIZE + 4),
            VDP2_VRAM + 4);

#ifdef LINES
        static const uint16_t bands[4] = {
                VDP2_RGB(31, 0, 0), VDP2_RGB(0, 31, 0), VDP2_RGB(0, 0, 31), VDP2_RGB(31, 31, 31)
        };
        uint16_t * const table = vdp2_vram(TABLE);

        for (uint32_t line = 0; line < SCREEN_LINES; line++) {
                table[line] = bands[line / BAND_LINES];
        }
        vdp2_back_lines(TABLE);
#else
        /* An odd offset is the even one below it */
        *vdp2_vram(TABLE) = 0;
        vdp2_back_color(TABLE + 1, ONE_COLOR);
        hx_test_equal("back.word", *vdp2_vram(TABLE), ONE_COLOR);
#endif

        /* From a display that is off. The mode is every field at once, each with a bit past
         * its field, which is dropped */
        vdp2_screen((vdp2_width_t)(8 | VDP2_WIDTH_640), (vdp2_height_t)(4 | VDP2_HEIGHT_240),
            (vdp2_interlace_t)(4 | VDP2_INTERLACE_DOUBLE), (vdp2_border_t)(2 | VDP2_BORDER_BACK));
        hx_test_equal("screen.tvmd", VDP2->tvmd, 0x81D2);

        /* From a display that is on, to the mode the picture is in */
        vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, BORDER);
        hx_test_equal("screen.again.tvmd", VDP2->tvmd,
            VDP2_TVMD_DISP | (BORDER == VDP2_BORDER_BACK ? VDP2_TVMD_BDCLMD : 0));

        /* The line a vertical blank starts on, as the V counter has it: the first on which
         * VBLANK reads 1 after a line of the picture */
        while (VDP2->tvstat & VDP2_TVSTAT_VBLANK) {
        }
        while (!(VDP2->tvstat & VDP2_TVSTAT_VBLANK)) {
        }
        const uint32_t blank = line();

        /* Called in the picture: the call returns on that line */
        while (VDP2->tvstat & VDP2_TVSTAT_VBLANK) {
        }
        vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, BORDER);
        hx_test_equal("screen.picture.line", line(), blank);

        /* Called in the blank, a line or more past its start: the call waits for the next
         * one, and returns on its first line */
        while (line() == blank) {
        }
        vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, BORDER);
        hx_test_equal("screen.blank.line", line(), blank);

        hx_test_end();
        for (;;) {
        }
}
