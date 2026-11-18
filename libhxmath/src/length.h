#ifndef HX_SRC_LENGTH_H
#define HX_SRC_LENGTH_H

#include <stdbool.h>
#include <stdint.h>

#include "tables.h"

/* What the normalize and the look-at share: sizes brought to the top of a register, and one over
 * a length by a table and multiplies, no divide. */

#define INLINE static inline __attribute__ ((always_inline))

INLINE uint32_t
magnitude(int32_t value)
{
        return (value < 0) ? (0U - (uint32_t)value) : (uint32_t)value;
}

INLINE uint32_t
high_product(uint32_t a, uint32_t b)
{
        return (uint32_t)(((uint64_t)a * b) >> 32);
}

/* Shifts three sizes together until the largest is in 2^29 to 2^30, and returns how far up they
 * went, below 0 for down. Not for three zeros. The steps are the same few for any input */
INLINE int32_t
raised(uint32_t *a, uint32_t *b, uint32_t *c)
{
        uint32_t x = *a;
        uint32_t y = *b;
        uint32_t z = *c;
        uint32_t all = x | y | z;
        int32_t count = 0;

#define RAISED_STEP(below, by)                                                 \
        if (all < (below)) {                                                   \
                x <<= (by);                                                    \
                y <<= (by);                                                    \
                z <<= (by);                                                    \
                all <<= (by);                                                  \
                count += (by);                                                 \
        }

        if (all >= 0x80000000U) {
                x >>= 2;
                y >>= 2;
                z >>= 2;
                count = -2;
        } else if (all >= 0x40000000U) {
                x >>= 1;
                y >>= 1;
                z >>= 1;
                count = -1;
        } else {
                RAISED_STEP(0x00004000U, 16)
                RAISED_STEP(0x00400000U, 8)
                RAISED_STEP(0x04000000U, 4)
                RAISED_STEP(0x10000000U, 2)
                RAISED_STEP(0x20000000U, 1)
        }

#undef RAISED_STEP

        *a = x;
        *b = y;
        *c = z;

        return count;
}

/* The sum of three squares over 2^32, each value under 2^30. Each square's high half by itself:
 * up to 3 under the whole sum's, which is 2^26 at least */
INLINE uint32_t
squares_high(uint32_t a, uint32_t b, uint32_t c)
{
        return high_product(a, a) + high_product(b, b) + high_product(c, c);
}

/* One over the root of value / 2^30 as a share of 65536, value 2^30 to 2^32: the table's entry
 * and its step by the 8 bits below. Up to 1 under it and 3.7 over */
INLINE uint32_t
reciprocal_root_guess(uint32_t value)
{
        const uint16_t * const entry = hx_priv_reciprocal_roots[(value >> 24) - 64U];

        return entry[0] - (((uint32_t)entry[1] * ((value >> 16) & 0xFFU)) >> 8);
}

/* The same as a share of 2^31: the guess, and one Newton step on it, the guess by
 * (3 - value guess^2) / 2. Up to 11 under it and 1 over */
INLINE uint32_t
reciprocal_root(uint32_t value)
{
        const uint32_t guess = reciprocal_root_guess(value);
        const uint32_t near_one = high_product(value, (uint32_t)(uint16_t)guess * (uint16_t)guess);

        return high_product(guess << 16, 0xC0000000U - near_one);
}

struct length {
        /* 2^60 over the length */
        uint32_t reciprocal;
        /* Twice the length */
        uint32_t twice;
};

/* The length whose square over 2^32 is given, 2^26 to 2^30: squares_high of sizes that "raised"
 * left */
INLINE struct length
length_of(uint32_t high)
{
        /* Into 2^30 to 2^32 by an even count, which the root halves */
        const bool small = high < 0x10000000U;
        const uint32_t value = small ? (high << 4) : (high << 2);
        const uint32_t reciprocal = reciprocal_root(value);
        const uint32_t root = high_product(value, reciprocal);
        struct length length;

        length.reciprocal = small ? reciprocal : (reciprocal >> 1);
        length.twice = small ? (root << 1) : (root << 2);

        return length;
}

#endif /* !HX_SRC_LENGTH_H */
