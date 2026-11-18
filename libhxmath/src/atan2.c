#include <stdint.h>

#include <hx/trig.h>

#include "divide.h"
#include "tables.h"

/* Folded into the first eighth of a turn, where the smaller over the larger is 0 to 1. The angle
 * is base plus the arc, or less it where sign is -1. The arc is read between two of the table's
 * entries */
hx_angle_t
hx_atan2(hx_fix16_t y, hx_fix16_t x)
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
        if (large == 0U) {
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

        /* The divide unit takes a divisor below 2^31 */
        if ((large & 0x80000000UL) != 0U) {
                small >>= 1;
                large >>= 1;
        }

        /* The ratio by 2^26: its top 10 bits pick the entry, the 16 below are the part between
         * it and the next. At a ratio of 1 there is no next and no part */
        const uint32_t ratio = hx_priv_divide(small >> 6, small << 26, large);
        const uint32_t index = ratio >> 16;
        const uint32_t part = ratio & 0xFFFFU;
        const uint32_t below = hx_priv_arcs[index];
        const uint32_t above = hx_priv_arcs[index + ((index < 1024U) ? 1U : 0U)];
        const int32_t arc = (int32_t)(below + ((((above - below) * part) + 0x8000U) >> 16));

        return (hx_angle_t)(base + ((arc ^ sign) - sign));
}
