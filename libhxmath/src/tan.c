#include <stdbool.h>
#include <stdint.h>

#include <hx/trig.h>

#include "divide.h"

/* An angle unit in radians, by 2^45 */
#define UNIT 3373259426UL

/* One over an angle unit as 16.16, by 2 */
#define OVER_UNIT 1367130551UL

/* Folded into the first quarter turn, and from there into its nearer half: x is the angle from 0
 * or the angle left to the quarter turn, an eighth of a turn at most, in radians by 2^32.
 *
 * With cot x = 1/x - x G(x^2):
 *   from 0              tan x = x / (1 - x^2 G), one divide
 *   to the quarter turn cot x = 1/x - x G. x is a whole number of angle units, so 1/x is one over
 *                       a unit divided by that number, taken in two divides for 17 places more */
hx_fix16_t
hx_tan(hx_angle_t angle)
{
        /* G's terms by 2^32, the last first: 1382/638512875, 2/93555, 1/4725, 2/945, 1/45, 1/3 */
        static const uint32_t terms[] = {
                9296UL, 91817UL, 908988UL, 9089878UL, 95443718UL, 1431655765UL
        };

        const uint32_t half = (uint32_t)angle & 0x7FFFU;
        const bool negative = half > 0x4000U;
        uint32_t steps = negative ? (0x8000U - half) : half;

        if (steps == 0x4000U) {
                return INT32_MAX;
        }
        if (steps == 0U) {
                return 0;
        }

        const bool from_zero = steps <= 0x2000U;

        if (!from_zero) {
                steps = 0x4000U - steps;
        }

        const uint32_t x = (uint32_t)(((uint64_t)steps * UNIT) >> 13);
        const uint32_t xx = (uint32_t)(((uint64_t)x * x) >> 32);
        uint32_t g = terms[0];

        for (uint32_t i = 1; i < (sizeof(terms) / sizeof(terms[0])); i++) {
                g = terms[i] + (uint32_t)(((uint64_t)g * xx) >> 32);
        }

        uint32_t result;

        if (from_zero) {
                /* The divisor by 2^31, to stay positive for the unit. Half of it rounds */
                const uint32_t below = (0U - (uint32_t)(((uint64_t)xx * g) >> 32)) >> 1;
                const uint64_t above = ((uint64_t)x << 15) + (below >> 1);

                result = hx_priv_divide((uint32_t)(above >> 32), (uint32_t)above, below);
        } else {
                const uint32_t first = hx_priv_divide(0U, OVER_UNIT, steps);
                const uint32_t left = OVER_UNIT - (first * steps);
                const uint32_t second = hx_priv_divide(0U, left << 16, steps);
                const uint32_t less = 2U * (uint32_t)(((uint64_t)x * g) >> 32);

                /* 16.16 by 2^17 */
                const uint64_t whole = (((uint64_t)first << 16) + second) - less;

                result = (uint32_t)((whole + 0x10000U) >> 17);
        }

        return negative ? -(hx_fix16_t)result : (hx_fix16_t)result;
}
