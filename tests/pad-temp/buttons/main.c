#include <stddef.h>
#include <stdint.h>

#include <hx/test.h>
#include <hx/vdp2.h>

/* hx_test_pad_temp, with a Saturn pad at the first port: each button on its own, in the order
 * below. A row takes the word read once a button is held, which is to be that button alone.
 *
 * The report is on screen from the start, and again after each row. On Beetle the test's
 * Makefile presses the buttons. On a console they are pressed by hand, in the same order: the
 * row a press gives is the next line on screen. */

static const struct {
        const char *name;
        uint32_t button;
} presses[] = {
        { "01.up", HX_TEST_PAD_UP_TEMP },
        { "02.down", HX_TEST_PAD_DOWN_TEMP },
        { "03.left", HX_TEST_PAD_LEFT_TEMP },
        { "04.right", HX_TEST_PAD_RIGHT_TEMP },
        { "05.start", HX_TEST_PAD_START_TEMP },
        { "06.a", HX_TEST_PAD_A_TEMP },
        { "07.b", HX_TEST_PAD_B_TEMP },
        { "08.c", HX_TEST_PAD_C_TEMP },
        { "09.x", HX_TEST_PAD_X_TEMP },
        { "10.y", HX_TEST_PAD_Y_TEMP },
        { "11.z", HX_TEST_PAD_Z_TEMP },
        { "12.l", HX_TEST_PAD_L_TEMP },
        { "13.r", HX_TEST_PAD_R_TEMP }
};

/* The pad once a frame, at the start of the vertical blank. A button's contacts settle well
 * within a frame, so two reads in a row do not see a press twice */
static uint32_t
pad(void)
{
        while (VDP2->tvstat & VDP2_TVSTAT_VBLANK) {
        }
        while (!(VDP2->tvstat & VDP2_TVSTAT_VBLANK)) {
        }
        return hx_test_pad_temp(0);
}

int
main(void)
{
        /* The pad at rest: there, with no button held. The second port is a value to read,
         * since a pad may be there or not */
        hx_test_equal("00.rest", hx_test_pad_temp(0), HX_TEST_PAD_THERE_TEMP);
        hx_test_note("port2", hx_test_pad_temp(1));
        hx_test_show();

        for (size_t press = 0; press < sizeof presses / sizeof presses[0]; press++) {
                uint32_t word;

                /* No button held, then one held */
                while (pad() & HX_TEST_PAD_BUTTONS_TEMP) {
                }
                do {
                        word = pad();
                } while (!(word & HX_TEST_PAD_BUTTONS_TEMP));

                hx_test_equal(presses[press].name, word,
                    HX_TEST_PAD_THERE_TEMP | presses[press].button);
                hx_test_show();
        }

        hx_test_end();
        for (;;) {
        }
}
