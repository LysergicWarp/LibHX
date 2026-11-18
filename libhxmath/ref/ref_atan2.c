#include <hx/ref.h>

#include "tables.h"

/* Folded into the first eighth of a turn, where the smaller over the larger is 0 to 1. The angle
 * is base plus the arc, or less it where sign is -1. */
hx_angle_t
hx_ref_atan2(hx_fix16_t y, hx_fix16_t x)
{
        uint32_t small = (y < 0) ? (0U - (uint32_t)y) : (uint32_t)y;
        uint32_t large = (x < 0) ? (0U - (uint32_t)x) : (uint32_t)x;
        int32_t base = 0;
        int32_t sign = 0;

        if (small > large) {
                const uint32_t swapped = small;

                small = large;
                large = swapped;
                base = 0x4000;
                sign = -1;
        }
        if (large == 0) {
                return 0;
        }
        if (x < 0) {
                base = 0x8000 - base;
                sign = ~sign;
        }
        if (y < 0) {
                base = -base;
                sign = ~sign;
        }

        /* 11 bits of the ratio, the last to round by */
        const uint32_t ratio = (uint32_t)(((uint64_t)small << 11) / large);
        const int32_t arc = hx_priv_arcs[(ratio + 1U) >> 1];

        return (hx_angle_t)(base + ((arc ^ sign) - sign));
}
