#include <hx/fix16.h>
#include <hx/ref.h>

#include "length.h"

/* The value is shifted up by an even count into 2^30 to 2^32 and its root guessed from the table,
 * never over it. What the guess's square leaves, over twice the guess, makes up the rest. A
 * multiply by scale shifts the result back by half the count and one. */
hx_fix16_t
hx_ref_sqrt(hx_fix16_t value)
{
        uint32_t scaled = (uint32_t)value;
        uint32_t scale = 0x80000000UL;

        if (value <= 0) {
                return 0;
        }

        if (scaled < 0x00010000UL) {
                scaled <<= 16;
                scale >>= 8;
        }
        if (scaled < 0x01000000UL) {
                scaled <<= 8;
                scale >>= 4;
        }
        if (scaled < 0x10000000UL) {
                scaled <<= 4;
                scale >>= 2;
        }
        if (scaled < 0x40000000UL) {
                scaled <<= 2;
                scale >>= 1;
        }

        const uint16_t * const entry = hx_priv_roots[scaled >> 24];
        const uint32_t guess = entry[0] + (((uint32_t)entry[1] * ((scaled >> 16) & 0xFFU)) >> 8);
        const uint32_t rest = scaled - (guess * guess);
        const uint32_t twice = (guess << 9) + ((rest << 8) / guess);

        return (hx_fix16_t)(((uint64_t)twice * scale) >> 32);
}

/* The value is shifted up the same way, and one over its root comes from length.h as a share of
 * 2^31. A multiply by scale takes it back up by half the count, to 16.16. */
hx_fix16_t
hx_ref_rsqrt(hx_fix16_t value)
{
        uint32_t scaled = (uint32_t)value;
        uint32_t scale = 0x00000400UL;

        if (value <= 0) {
                return 0;
        }

        if (scaled < 0x00010000UL) {
                scaled <<= 16;
                scale <<= 8;
        }
        if (scaled < 0x01000000UL) {
                scaled <<= 8;
                scale <<= 4;
        }
        if (scaled < 0x10000000UL) {
                scaled <<= 4;
                scale <<= 2;
        }
        if (scaled < 0x40000000UL) {
                scaled <<= 2;
                scale <<= 1;
        }

        return (hx_fix16_t)high_product(reciprocal_root(scaled), scale);
}
