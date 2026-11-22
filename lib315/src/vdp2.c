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
         * written, but for the reserved one. A cycle pattern of 0 would read NBG0's pattern
         * names in every timing, so the patterns are no access throughout */
        for (uint32_t address = VDP2_CYCA0L; address <= VDP2_COBB; address += 2) {
                if (address != RESERVED_FE) {
                        *(volatile uint16_t *)address = (address <= VDP2_CYCB1U) ? 0xFFFF : 0;
                }
        }

        /* An increment of 0 gives every dot of NBG0 or NBG1 the same place in its map */
        VDP2->zmxin0 = 1;
        VDP2->zmyin0 = 1;
        VDP2->zmxin1 = 1;
        VDP2->zmyin1 = 1;
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

void
vdp2_cycle(vdp2_bank_t bank, uint32_t pattern)
{
        /* Two words a bank, from CYCA0L: T0 to T3, then T4 to T7 */
        const uint32_t address = VDP2_CYCA0L + (((uint32_t)bank & 3) << 2);

        *(volatile uint16_t *)address = (uint16_t)(pattern >> 16);
        *(volatile uint16_t *)(address + 2) = (uint16_t)pattern;
}

/* The size of a page of pattern names as a shift: 64 by 64 names of 8 by 8 or 32 by 32 of 16
 * by 16, each of one word or two */
static uint32_t
page_shift(const vdp2_nbg_t *screen)
{
        const uint32_t two_words = ((uint32_t)screen->names & 2) == 0;
        const uint32_t four_cells = (uint32_t)screen->character & 1;

        return 13 + two_words - (four_cells << 1);
}

/* A plane's place as the map registers have it: a count of pages, 6 bits in the plane's own
 * field and 3 above them in the screen's map offset */
static uint32_t
plane_page(const vdp2_nbg_t *screen, uint32_t plane)
{
        return (screen->planes[plane] & (VDP2_VRAM_SIZE - 1)) >> page_shift(screen);
}

void
vdp2_scroll_format(const vdp2_scroll_t *format)
{
        const vdp2_nbg_t * const nbg = format->nbg;
        uint32_t plane_sizes = 0;
        uint32_t map_offsets = 0;
        uint32_t color_bases = 0;

        VDP2->chctla = (uint16_t)((((uint32_t)nbg[0].colors << 4) & VDP2_CHCTLA_N0CHCN)
            | ((uint32_t)nbg[0].character & VDP2_CHCTLA_N0CHSZ)
            | (((uint32_t)nbg[1].colors << 12) & VDP2_CHCTLA_N1CHCN)
            | (((uint32_t)nbg[1].character << 8) & VDP2_CHCTLA_N1CHSZ));
        VDP2->chctlb = (uint16_t)((((uint32_t)nbg[2].colors << 1) & VDP2_CHCTLB_N2CHCN)
            | ((uint32_t)nbg[2].character & VDP2_CHCTLB_N2CHSZ)
            | (((uint32_t)nbg[3].colors << 5) & VDP2_CHCTLB_N3CHCN)
            | (((uint32_t)nbg[3].character << 4) & VDP2_CHCTLB_N3CHSZ));

        for (uint32_t screen = 0; screen < 4; screen++) {
                const vdp2_nbg_t * const from = &nbg[screen];
                const uint32_t a = plane_page(from, 0);
                /* PNCN0 to PNCN3 are four words in a row, and the map registers two words
                 * a screen: planes A and B, then C and D */
                const uint32_t names = VDP2_PNCN0 + (screen << 1);
                const uint32_t map = VDP2_MPABN0 + (screen << 2);

                *(volatile uint16_t *)names = (uint16_t)((((uint32_t)from->names << 14)
                    & (VDP2_PNCN_PNB | VDP2_PNCN_CNSM))
                    | (from->supplement & (VDP2_PNCN_SPR | VDP2_PNCN_SCC | VDP2_PNCN_SPLT
                    | VDP2_PNCN_SCN)));
                *(volatile uint16_t *)map = (uint16_t)(((plane_page(from, 1) & 63) << 8)
                    | (a & 63));
                *(volatile uint16_t *)(map + 2) = (uint16_t)(((plane_page(from, 3) & 63) << 8)
                    | (plane_page(from, 2) & 63));

                plane_sizes |= ((uint32_t)from->plane & 3) << (screen << 1);
                map_offsets |= ((a >> 6) & 7) << (screen << 2);
                color_bases |= ((uint32_t)from->color_base & 7) << (screen << 2);
        }
        VDP2->plsz = (uint16_t)plane_sizes;
        VDP2->mpofn = (uint16_t)map_offsets;
        VDP2->craofa = (uint16_t)color_bases;
}

void
vdp2_show(uint32_t screens)
{
        VDP2->bgon = (uint16_t)screens;
}

void
vdp2_nbg_priority(uint32_t nbg0, uint32_t nbg1, uint32_t nbg2, uint32_t nbg3)
{
        VDP2->prina = (uint16_t)(((nbg1 & 7) << 8) | (nbg0 & 7));
        VDP2->prinb = (uint16_t)(((nbg3 & 7) << 8) | (nbg2 & 7));
}
