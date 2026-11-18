#include <hx/ref.h>

#include "length.h"

hx_fix16_t
hx_ref_vec3_dot(const hx_vec3_t *a, const hx_vec3_t *b)
{
        /* Unsigned so that the sum wraps */
        const uint64_t sum = (uint64_t)((int64_t)a->x * b->x) + (uint64_t)((int64_t)a->y * b->y) +
            (uint64_t)((int64_t)a->z * b->z);

        return (hx_fix16_t)(uint32_t)(sum >> 16);
}

/* a by b less c by d, as 16.16. Unsigned so that it wraps */
static inline hx_fix16_t
products_apart(int32_t a, int32_t b, int32_t c, int32_t d)
{
        const uint64_t apart = (uint64_t)((int64_t)a * b) - (uint64_t)((int64_t)c * d);

        return (hx_fix16_t)(((uint32_t)(apart >> 32) << 16) | ((uint32_t)apart >> 16));
}

void
hx_ref_vec3_cross(const hx_vec3_t *a, const hx_vec3_t *b, hx_vec3_t *result)
{
        result->x = products_apart(a->y, b->z, a->z, b->y);
        result->y = products_apart(a->z, b->x, a->x, b->z);
        result->z = products_apart(a->x, b->y, a->y, b->x);
}

/* Half of the last place kept, for the form that rounds to the nearest */
#define HALF 0x80U

INLINE void
normalized(const hx_vec3_t *v, hx_vec3_t *result, uint32_t half)
{
        const int32_t x = v->x;
        const int32_t y = v->y;
        const int32_t z = v->z;
        uint32_t ux = magnitude(x);
        uint32_t uy = magnitude(y);
        uint32_t uz = magnitude(z);

        if ((ux | uy | uz) == 0) {
                result->x = 0;
                result->y = 0;
                result->z = 0;

                return;
        }

        raised(&ux, &uy, &uz);

        /* 2^56 over the length, so a coordinate by it is the result by 2^40: the high half of
         * the product is then a byte from the result */
        const uint32_t reciprocal = length_of(squares_high(ux, uy, uz)).reciprocal >> 4;

        ux = (high_product(ux, reciprocal) + half) >> 8;
        uy = (high_product(uy, reciprocal) + half) >> 8;
        uz = (high_product(uz, reciprocal) + half) >> 8;

        result->x = (x < 0) ? -(int32_t)ux : (int32_t)ux;
        result->y = (y < 0) ? -(int32_t)uy : (int32_t)uy;
        result->z = (z < 0) ? -(int32_t)uz : (int32_t)uz;
}

void
hx_ref_vec3_normalize(const hx_vec3_t *v, hx_vec3_t *result)
{
        normalized(v, result, 0);
}

void
hx_ref_vec3_normalize_round(const hx_vec3_t *v, hx_vec3_t *result)
{
        normalized(v, result, HALF);
}
