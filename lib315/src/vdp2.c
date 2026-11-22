#include <hx/vdp2.h>

/* The word between PRIR and CCRSA, which is reserved */
#define RESERVED_FE (VDP2_PRIR + 2)

void
vdp2_start(void)
{
        /* The display off first, so nothing of what follows shows */
        VDP2->tvmd = 0;
        VDP2->exten = 0;
        VDP2->vrsize = 0;
        VDP2->ramctl = 0;

        /* From the cycle patterns to the colour offsets, every word is a register that is
         * written, but for the reserved one */
        for (uint32_t address = VDP2_CYCA0L; address <= VDP2_COBB; address += 2) {
                if (address != RESERVED_FE) {
                        *(volatile uint16_t *)address = 0;
                }
        }
}

void
vdp2_screen(vdp2_width_t width, vdp2_height_t height, vdp2_interlace_t interlace,
    vdp2_border_t border)
{
        const uint32_t mode = VDP2_TVMD_DISP
            | (((uint32_t)border << 8) & VDP2_TVMD_BDCLMD)
            | (((uint32_t)interlace << 6) & VDP2_TVMD_LSMD)
            | (((uint32_t)height << 4) & VDP2_TVMD_VRESO)
            | ((uint32_t)width & VDP2_TVMD_HRESO);

        /* With the display off the chip is in blank on every line, and VBLANK may read 1
         * throughout. So only a display that is on has a blank to leave first */
        if (VDP2->tvmd & VDP2_TVMD_DISP) {
                while (VDP2->tvstat & VDP2_TVSTAT_VBLANK) {
                }
        }
        while (!(VDP2->tvstat & VDP2_TVSTAT_VBLANK)) {
        }
        VDP2->tvmd = (uint16_t)mode;
}

uint32_t
vdp2_pal(void)
{
        return VDP2->tvstat & VDP2_TVSTAT_PAL;
}

/* Puts the back screen's table at offset. The address counts words, its top 3 bits beside the
 * mode */
static void
back_table(uint32_t offset, uint32_t mode)
{
        const uint32_t word = (offset & (VDP2_VRAM_SIZE - 2)) >> 1;

        VDP2->bktau = (uint16_t)(mode | ((word >> 16) & VDP2_BKTAU_BKTA));
        VDP2->bktal = (uint16_t)word;
}

void
vdp2_back_color(uint32_t offset, uint16_t color)
{
        /* The colour ahead of the table's address, so the screen shows no word left there.
         * Written as the registers are, so the compiler keeps the order */
        *(volatile uint16_t *)vdp2_vram(offset) = color;
        back_table(offset, 0);
}

void
vdp2_back_lines(uint32_t offset)
{
        back_table(offset, VDP2_BKTAU_BKCLMD);
}
