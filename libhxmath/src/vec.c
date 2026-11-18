#include <hx/vec2.h>
#include <hx/vec3.h>

#include "length.h"

/* The size of b less a, taken whole: under 2^32 for any two values */
INLINE uint32_t
apart(int32_t a, int32_t b)
{
        return (b > a) ? ((uint32_t)b - (uint32_t)a) : ((uint32_t)a - (uint32_t)b);
}

/* The value shifted down by count, 0 to 31. The steps are the same few for any count */
INLINE uint32_t
lowered(uint32_t value, uint32_t count)
{
        if ((count & 16U) != 0) {
                value >>= 16;
        }
        if ((count & 8U) != 0) {
                value >>= 8;
        }
        if ((count & 4U) != 0) {
                value >>= 4;
        }
        if ((count & 2U) != 0) {
                value >>= 2;
        }
        if ((count & 1U) != 0) {
                value >>= 1;
        }

        return value;
}

/* The sum of three squares, whole */
INLINE uint64_t
squares(uint32_t a, uint32_t b, uint32_t c)
{
        return ((uint64_t)a * a) + ((uint64_t)b * b) + ((uint64_t)c * c);
}

/* A guess at a root and one Newton step on it against the root's exact square: the guess plus
 * what its square leaves over twice the guess. Twice the guess is 2^62 over the reciprocal. With
 * the guess within 64 of the root the result is within 1 of it */
INLINE uint32_t
stepped(uint32_t guess, uint32_t reciprocal, uint64_t square)
{
        const uint64_t left = square - ((uint64_t)guess * guess);
        /* What is left over 2^9, under 2^31 either way */
        const uint32_t scaled = ((uint32_t)(left >> 32) << 23) | ((uint32_t)left >> 9);
        const bool over = (int32_t)scaled < 0;
        const uint32_t step = high_product(over ? (0U - scaled) : scaled, reciprocal) >> 21;

        return over ? (guess - step) : (guess + step);
}

/* A root within 1 of the root of square rounded down, made that root */
INLINE uint32_t
rounded_down(uint32_t root, uint64_t square)
{
        const uint64_t below = (uint64_t)root * root;

        if (below > square) {
                return root - 1U;
        }
        /* The next root's square is below's and twice the root and one, which can pass 2^64 */
        if ((square - below) > ((uint64_t)root << 1)) {
                return root + 1U;
        }

        return root;
}

/* Raised by this much or more, length.h's twice the length is within 1 of the length once shifted
 * back down: it is within 32 of twice the raised sizes' length */
#define SHORT 5

/* The length of three sizes, not all 0, in their units and rounded down, while it is under 2^32.
 * square is the sum of their squares.
 *
 * length.h's length comes from the squares' high halves and a table with one Newton step, within
 * about 1/2^26 of the exact one. For a long vector one more step against the whole squares brings
 * it within 1, and a compare with them makes it exact */
INLINE uint32_t
length_exact(uint32_t x, uint32_t y, uint32_t z, uint64_t square)
{
        uint32_t a = x;
        uint32_t b = y;
        uint32_t c = z;
        const int32_t count = raised(&a, &b, &c);
        const uint64_t aa = (uint64_t)a * a;
        const uint64_t bb = (uint64_t)b * b;
        const uint64_t cc = (uint64_t)c * c;
        const struct length length = length_of((uint32_t)(aa >> 32) + (uint32_t)(bb >> 32) +
            (uint32_t)(cc >> 32));
        uint32_t root;

        if (count >= SHORT) {
                root = lowered(length.twice, (uint32_t)count + 1U);
        } else if (count >= 0) {
                /* The sizes went up whole: twice their length against four times their squares,
                 * then back down by the count and the one */
                root = lowered(stepped(length.twice, length.reciprocal, (aa + bb + cc) << 2),
                    (uint32_t)count + 1U);
        } else if (count == -1) {
                /* They went down and lost bits: the length itself against the squares of the
                 * sizes given. Down by 1, twice their length is it, down by 2, four times, held
                 * under 2^32 for a length just under it */
                root = stepped(length.twice, length.reciprocal, square);
        } else {
                const uint32_t guess = (length.twice < 0x80000000U) ? (length.twice << 1) :
                    0xFFFFFFFFU;

                root = stepped(guess, length.reciprocal >> 1, square);
        }

        return rounded_down(root, square);
}

hx_fix16_t
hx_vec3_length(const hx_vec3_t *v)
{
        const uint32_t x = magnitude(v->x);
        const uint32_t y = magnitude(v->y);
        const uint32_t z = magnitude(v->z);

        if ((x | y | z) == 0) {
                return 0;
        }

        return (hx_fix16_t)length_exact(x, y, z, hx_vec3_length_squared(v));
}

hx_fix16_t
hx_vec3_distance(const hx_vec3_t *a, const hx_vec3_t *b)
{
        const uint32_t x = apart(a->x, b->x);
        const uint32_t y = apart(a->y, b->y);
        const uint32_t z = apart(a->z, b->z);

        if ((x | y | z) == 0) {
                return 0;
        }

        return (hx_fix16_t)length_exact(x, y, z, squares(x, y, z));
}

hx_fix16_t
hx_vec2_length(const hx_vec2_t *v)
{
        const uint32_t x = magnitude(v->x);
        const uint32_t y = magnitude(v->y);

        if ((x | y) == 0) {
                return 0;
        }

        return (hx_fix16_t)length_exact(x, y, 0, hx_vec2_length_squared(v));
}

hx_fix16_t
hx_vec2_distance(const hx_vec2_t *a, const hx_vec2_t *b)
{
        const uint32_t x = apart(a->x, b->x);
        const uint32_t y = apart(a->y, b->y);

        if ((x | y) == 0) {
                return 0;
        }

        return (hx_fix16_t)length_exact(x, y, 0, squares(x, y, 0));
}

/* Half of the last place kept, for the form that rounds to the nearest */
#define HALF 0x80U

/* The 3D normalize's C reference with no z */
INLINE void
normalized(const hx_vec2_t *v, hx_vec2_t *result, uint32_t half)
{
        const int32_t x = v->x;
        const int32_t y = v->y;
        uint32_t ux = magnitude(x);
        uint32_t uy = magnitude(y);
        uint32_t uz = 0;

        if ((ux | uy) == 0) {
                result->x = 0;
                result->y = 0;

                return;
        }

        raised(&ux, &uy, &uz);

        /* 2^56 over the length, so a coordinate by it is the result by 2^40: the high half of
         * the product is then a byte from the result */
        const uint32_t reciprocal = length_of(squares_high(ux, uy, 0)).reciprocal >> 4;

        ux = (high_product(ux, reciprocal) + half) >> 8;
        uy = (high_product(uy, reciprocal) + half) >> 8;

        result->x = (x < 0) ? -(int32_t)ux : (int32_t)ux;
        result->y = (y < 0) ? -(int32_t)uy : (int32_t)uy;
}

void
hx_vec2_normalize(const hx_vec2_t *v, hx_vec2_t *result)
{
        normalized(v, result, 0);
}

void
hx_vec2_normalize_round(const hx_vec2_t *v, hx_vec2_t *result)
{
        normalized(v, result, HALF);
}
