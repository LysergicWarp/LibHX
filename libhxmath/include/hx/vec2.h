#ifndef HX_VEC2_H
#define HX_VEC2_H

#include <hx/types.h>
#include <hx/fix16.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The 3D set again in the plane, each function as its 3D one. There is no fast length in 2D */

/* a plus b. Each coordinate wraps. The result may be a or b */
static inline void
hx_vec2_add(const hx_vec2_t *a, const hx_vec2_t *b, hx_vec2_t *result)
{
        result->x = (hx_fix16_t)((uint32_t)a->x + (uint32_t)b->x);
        result->y = (hx_fix16_t)((uint32_t)a->y + (uint32_t)b->y);
}

/* a less b. Each coordinate wraps. The result may be a or b */
static inline void
hx_vec2_sub(const hx_vec2_t *a, const hx_vec2_t *b, hx_vec2_t *result)
{
        result->x = (hx_fix16_t)((uint32_t)a->x - (uint32_t)b->x);
        result->y = (hx_fix16_t)((uint32_t)a->y - (uint32_t)b->y);
}

/* v by a scalar, each coordinate as hx_mul gives it: rounded down, and it wraps. The result may
 * be v */
static inline void
hx_vec2_scale(const hx_vec2_t *v, hx_fix16_t scalar, hx_vec2_t *result)
{
        result->x = hx_mul(v->x, scalar);
        result->y = hx_mul(v->y, scalar);
}

/* a plus b by a scalar: each coordinate is a's plus hx_mul of b's, and it wraps. The result may
 * be a or b */
static inline void
hx_vec2_add_scaled(const hx_vec2_t *a, const hx_vec2_t *b, hx_fix16_t scalar, hx_vec2_t *result)
{
        result->x = (hx_fix16_t)((uint32_t)a->x + (uint32_t)hx_mul(b->x, scalar));
        result->y = (hx_fix16_t)((uint32_t)a->y + (uint32_t)hx_mul(b->y, scalar));
}

/* The two products summed whole, then rounded down once. SR's S bit is taken as clear */
static inline hx_fix16_t
hx_vec2_dot(const hx_vec2_t *a, const hx_vec2_t *b)
{
        uint32_t high;
        uint32_t low;

        __asm__ ("clrmac\n"
                 "\tmac.l @%[a]+, @%[b]+\n"
                 "\tmac.l @%[a]+, @%[b]+\n"
                 "\tsts mach, %[high]\n"
                 "\tsts macl, %[low]\n"
                 "\txtrct %[high], %[low]"
                 : [a] "+r" (a), [b] "+r" (b), [high] "=&r" (high), [low] "=&r" (low)
                 : "m" (*a), "m" (*b)
                 : "mach", "macl");

        return (hx_fix16_t)low;
}

/* a cross b in the plane: a's x by b's y less a's y by b's x, the z of the 3D cross. Above 0 when
 * b is a positive turn from a, x toward y. The two products taken apart whole, then rounded down
 * once */
static inline hx_fix16_t
hx_vec2_cross(const hx_vec2_t *a, const hx_vec2_t *b)
{
        /* Unsigned so that it wraps */
        const uint64_t apart = (uint64_t)((int64_t)a->x * b->y) - (uint64_t)((int64_t)a->y * b->x);

        return (hx_fix16_t)(((uint32_t)(apart >> 32) << 16) | ((uint32_t)apart >> 16));
}

/* The squared length, exact for any vector: the sum of the squares with 32 fraction bits. SR's S
 * bit is taken as clear */
static inline uint64_t
hx_vec2_length_squared(const hx_vec2_t *v)
{
        const hx_vec2_t *a = v;
        const hx_vec2_t *b = v;
        uint32_t high;
        uint32_t low;

        __asm__ ("clrmac\n"
                 "\tmac.l @%[a]+, @%[b]+\n"
                 "\tmac.l @%[a]+, @%[b]+\n"
                 "\tsts mach, %[high]\n"
                 "\tsts macl, %[low]"
                 : [a] "+r" (a), [b] "+r" (b), [high] "=&r" (high), [low] "=&r" (low)
                 : "m" (*v)
                 : "mach", "macl");

        return ((uint64_t)high << 32) | low;
}

/* The squared length as 16.16, hx_vec2_dot of v and itself. It fits while the length is under
 * 181, past that it wraps */
static inline hx_fix16_t
hx_vec2_length_squared_fix16(const hx_vec2_t *v)
{
        return hx_vec2_dot(v, v);
}

/* The length rounded down, exact for any vector. Past 32768 it wraps: read as unsigned it holds
 * for every vector */
hx_fix16_t hx_vec2_length(const hx_vec2_t *v);

/* The length of b less a, as hx_vec2_length gives it. The difference is taken whole, so read as
 * unsigned the result holds for two points under 65536 apart */
hx_fix16_t hx_vec2_distance(const hx_vec2_t *a, const hx_vec2_t *b);

/* v at the length of one. A vector of any size, (0, 0) stays as it is. The result may be v.
 *
 * The plain one rounds each coordinate toward zero: a vector along an axis may come out 1/65536
 * short of one. The other rounds to the nearest, and such a vector comes out as one exactly */
void hx_vec2_normalize(const hx_vec2_t *v, hx_vec2_t *result);
void hx_vec2_normalize_round(const hx_vec2_t *v, hx_vec2_t *result);

#ifdef __cplusplus
}
#endif

#endif /* !HX_VEC2_H */
