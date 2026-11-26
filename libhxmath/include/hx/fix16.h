#ifndef HX_FIX16_H
#define HX_FIX16_H

#include <hx/types.h>
#include <hx/hw/sh2/divu.h>

/** @file
 * 16.16 arithmetic: bounds, rounding, products, divides and roots. */

#ifdef __cplusplus
extern "C" {
#endif

/** The most negative value comes back as it is */
static inline hx_fix16_t
hx_abs(hx_fix16_t value)
{
        return (value < 0) ? (hx_fix16_t)(0U - (uint32_t)value) : value;
}

static inline hx_fix16_t
hx_min(hx_fix16_t a, hx_fix16_t b)
{
        return (a < b) ? a : b;
}

static inline hx_fix16_t
hx_max(hx_fix16_t a, hx_fix16_t b)
{
        return (a > b) ? a : b;
}

/** low is at most high */
static inline hx_fix16_t
hx_clamp(hx_fix16_t value, hx_fix16_t low, hx_fix16_t high)
{
        return (value < low) ? low : ((value > high) ? high : value);
}

/** The whole number at or below the value */
static inline hx_fix16_t
hx_floor(hx_fix16_t value)
{
        return (hx_fix16_t)((uint32_t)value & 0xFFFF0000UL);
}

/** The whole number at or above the value. Wraps above 32767 */
static inline hx_fix16_t
hx_ceil(hx_fix16_t value)
{
        return (hx_fix16_t)(((uint32_t)value + 0xFFFFUL) & 0xFFFF0000UL);
}

/** What the value is above its floor: 0 or more, and less than 1 */
static inline hx_fix16_t
hx_frac(hx_fix16_t value)
{
        return (hx_fix16_t)((uint32_t)value & 0xFFFFUL);
}

/** Rounds down */
static inline hx_fix16_t
hx_mul(hx_fix16_t a, hx_fix16_t b)
{
        const int64_t product = (int64_t)a * b;

        return (hx_fix16_t)(((uint32_t)(product >> 32) << 16) | ((uint32_t)product >> 16));
}

/** To the nearest, a half goes up */
static inline hx_fix16_t
hx_mul_round(hx_fix16_t a, hx_fix16_t b)
{
        const int64_t product = ((int64_t)a * b) + 0x8000;

        return (hx_fix16_t)(((uint32_t)(product >> 32) << 16) | ((uint32_t)product >> 16));
}

/** a at a t of 0, b at a t of 1, the straight line through the two elsewhere. The part added to
 * a is rounded down. b less a has to fit in 32 bits, and so has the result: one that does not
 * wraps */
static inline hx_fix16_t
hx_lerp(hx_fix16_t a, hx_fix16_t b, hx_fix16_t t)
{
        const hx_fix16_t apart = (hx_fix16_t)((uint32_t)b - (uint32_t)a);

        return (hx_fix16_t)((uint32_t)a + (uint32_t)hx_mul(apart, t));
}

/** @name Divides
 * The divides use the divide unit, which holds one divide at a time: not for an interrupt handler,
 * and no other divide between a start and its collect. They round toward zero. A zero divisor or
 * a quotient out of range gives what the unit gives. A function elsewhere in the library that
 * uses the unit says so, and the same holds for it. */
/** @{ */

/** Starts a / b. The unit works for 39 clocks, the caller may do other work meanwhile */
static inline void
hx_div_start(hx_fix16_t a, hx_fix16_t b)
{
        volatile struct sh2_divu * const divu = SH2_DIVU;

        divu->dvsr = b;
        divu->dvdnth = a >> 16;
        divu->dvdntl = (int32_t)((uint32_t)a << 16);
}

/** The quotient of the divide started, waiting for it if need be */
static inline hx_fix16_t
hx_div_collect(void)
{
        return SH2_DIVU->dvdntl;
}

static inline hx_fix16_t
hx_div(hx_fix16_t a, hx_fix16_t b)
{
        hx_div_start(a, b);

        return hx_div_collect();
}

/** @} */

/** 0 for a value below 0. Never under the root, at most 1/65536 over */
hx_fix16_t hx_sqrt(hx_fix16_t value);

/* The root of a value's top 8 bits by 4096, rounded down, and the step to the next */
extern const uint16_t hx_priv_roots[256][2];

/** A root that gives up places for speed: never over the root, up to 3% of it under. 0 for a
 * value below 0.
 *
 * Below 1/256 it is the root rounded down, and so is the root of a value below 1 with only its
 * top 8 bits set. From 1 up such a value's root is up to 255/65536 under.
 *
 * The top 8 bits of the value from a nibble's edge are read off the root's table, and the
 * entry's shift makes up for where they were */
static inline hx_fix16_t
hx_sqrt_fast(hx_fix16_t value)
{
        const uint32_t bits = (uint32_t)value;

        if (value <= 0) {
                return 0;
        }

        if (bits < 0x00010000UL) {
                if (bits < 0x00000100UL) {
                        return (hx_fix16_t)(hx_priv_roots[bits][0] >> 4);
                }
                if (bits < 0x00001000UL) {
                        return (hx_fix16_t)(hx_priv_roots[bits >> 4][0] >> 2);
                }

                return (hx_fix16_t)hx_priv_roots[bits >> 8][0];
        }

        if (bits < 0x01000000UL) {
                if (bits < 0x00100000UL) {
                        return (hx_fix16_t)((uint32_t)hx_priv_roots[bits >> 12][0] << 2);
                }

                return (hx_fix16_t)((uint32_t)hx_priv_roots[bits >> 16][0] << 4);
        }

        if (bits < 0x10000000UL) {
                return (hx_fix16_t)((uint32_t)hx_priv_roots[bits >> 20][0] << 6);
        }

        return (hx_fix16_t)((uint32_t)hx_priv_roots[bits >> 24][0] << 8);
}

/** One over the root, at most 1/65536 from the exact one rounded down, on either side. A power
 * of 4 comes out 1/65536 short. 0 for a value of 0 or below. The largest result is 256 less
 * 1/65536, for 1/65536 */
hx_fix16_t hx_rsqrt(hx_fix16_t value);

#ifdef __cplusplus
}
#endif

#endif /* !HX_FIX16_H */
