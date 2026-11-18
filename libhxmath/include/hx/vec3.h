#ifndef HX_VEC3_H
#define HX_VEC3_H

#include <hx/types.h>
#include <hx/fix16.h>

/* a plus b. Each coordinate wraps. The result may be a or b */
static inline void
hx_vec3_add(const hx_vec3_t *a, const hx_vec3_t *b, hx_vec3_t *result)
{
        result->x = (hx_fix16_t)((uint32_t)a->x + (uint32_t)b->x);
        result->y = (hx_fix16_t)((uint32_t)a->y + (uint32_t)b->y);
        result->z = (hx_fix16_t)((uint32_t)a->z + (uint32_t)b->z);
}

/* a less b. Each coordinate wraps. The result may be a or b */
static inline void
hx_vec3_sub(const hx_vec3_t *a, const hx_vec3_t *b, hx_vec3_t *result)
{
        result->x = (hx_fix16_t)((uint32_t)a->x - (uint32_t)b->x);
        result->y = (hx_fix16_t)((uint32_t)a->y - (uint32_t)b->y);
        result->z = (hx_fix16_t)((uint32_t)a->z - (uint32_t)b->z);
}

/* v by a scalar, each coordinate as hx_mul gives it: rounded down, and it wraps. The result may
 * be v */
static inline void
hx_vec3_scale(const hx_vec3_t *v, hx_fix16_t scalar, hx_vec3_t *result)
{
        result->x = hx_mul(v->x, scalar);
        result->y = hx_mul(v->y, scalar);
        result->z = hx_mul(v->z, scalar);
}

/* a plus b by a scalar: each coordinate is a's plus hx_mul of b's, and it wraps. The result may
 * be a or b */
static inline void
hx_vec3_add_scaled(const hx_vec3_t *a, const hx_vec3_t *b, hx_fix16_t scalar, hx_vec3_t *result)
{
        result->x = (hx_fix16_t)((uint32_t)a->x + (uint32_t)hx_mul(b->x, scalar));
        result->y = (hx_fix16_t)((uint32_t)a->y + (uint32_t)hx_mul(b->y, scalar));
        result->z = (hx_fix16_t)((uint32_t)a->z + (uint32_t)hx_mul(b->z, scalar));
}

/* The three products summed whole, then rounded down once. SR's S bit is taken as clear */
static inline hx_fix16_t
hx_vec3_dot(const hx_vec3_t *a, const hx_vec3_t *b)
{
        uint32_t high;
        uint32_t low;

        __asm__ ("clrmac\n"
                 "\tmac.l @%[a]+, @%[b]+\n"
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

/* The squared length, exact for any vector: the sum of the squares with 32 fraction bits. SR's S
 * bit is taken as clear */
static inline uint64_t
hx_vec3_length_squared(const hx_vec3_t *v)
{
        const hx_vec3_t *a = v;
        const hx_vec3_t *b = v;
        uint32_t high;
        uint32_t low;

        __asm__ ("clrmac\n"
                 "\tmac.l @%[a]+, @%[b]+\n"
                 "\tmac.l @%[a]+, @%[b]+\n"
                 "\tmac.l @%[a]+, @%[b]+\n"
                 "\tsts mach, %[high]\n"
                 "\tsts macl, %[low]"
                 : [a] "+r" (a), [b] "+r" (b), [high] "=&r" (high), [low] "=&r" (low)
                 : "m" (*v)
                 : "mach", "macl");

        return ((uint64_t)high << 32) | low;
}

/* The squared length as 16.16, hx_vec3_dot of v and itself. It fits while the length is under
 * 181, past that it wraps */
static inline hx_fix16_t
hx_vec3_length_squared_fix16(const hx_vec3_t *v)
{
        return hx_vec3_dot(v, v);
}

/* The length rounded down, exact for any vector. Past 32768 it wraps: read as unsigned it holds
 * for every vector */
hx_fix16_t hx_vec3_length(const hx_vec3_t *v);

/* A length that gives up places for speed, as hx_sqrt_fast does: never over the length, and from
 * a length of 1/256 up at most 3% of it under. Below that it is up to 6% under, below 16/65536
 * it is the length rounded down, and it is 0 for (0, 0, 0) alone. Past 32768 it wraps as
 * hx_vec3_length does.
 *
 * hx_sqrt_fast's root of the squared length, which has 32 fraction bits. Below a length of about
 * 0.7 the squared length is rooted as it is, read as 16.16: the root is the length by 256, so it
 * goes down by 8. Up to a length of 181 it is cut to 16.16 and rooted. Past that the whole part
 * is rooted alone: the root of a whole number read as 16.16 is its root by 1/256, so it goes up
 * by 8. The whole part can pass hx_fix16_t, so from 2^30 up a quarter of it is rooted and goes up
 * by 9 */
static inline hx_fix16_t
hx_vec3_length_fast(const hx_vec3_t *v)
{
        const uint64_t square = hx_vec3_length_squared(v);
        const uint32_t whole = (uint32_t)(square >> 32);
        /* The squared length cut to 16.16, while the whole part is under 32768 */
        const uint32_t cut = (whole << 16) | ((uint32_t)square >> 16);

        if (whole >= 0x40000000U) {
                return (hx_fix16_t)((uint32_t)hx_sqrt_fast((hx_fix16_t)(whole >> 2)) << 9);
        }
        if (whole >= 0x8000U) {
                return (hx_fix16_t)((uint32_t)hx_sqrt_fast((hx_fix16_t)whole) << 8);
        }
        if (cut >= 0x8000U) {
                return hx_sqrt_fast((hx_fix16_t)cut);
        }

        return (hx_fix16_t)((uint32_t)hx_sqrt_fast((hx_fix16_t)(uint32_t)square) >> 8);
}

/* The length of b less a, as hx_vec3_length gives it. The difference is taken whole, so read as
 * unsigned the result holds for two points under 65536 apart */
hx_fix16_t hx_vec3_distance(const hx_vec3_t *a, const hx_vec3_t *b);

/* a cross b: x by y is z. Each coordinate is two products taken apart whole, then rounded down
 * once. The result is neither a nor b */
void hx_vec3_cross(const hx_vec3_t *a, const hx_vec3_t *b, hx_vec3_t *result);

/* v at the length of one. A vector of any size, (0, 0, 0) stays as it is. The result may be v.
 *
 * The plain one rounds each coordinate toward zero: a vector along an axis may come out 1/65536
 * short of one. The other rounds to the nearest, and such a vector comes out as one exactly */
void hx_vec3_normalize(const hx_vec3_t *v, hx_vec3_t *result);
void hx_vec3_normalize_round(const hx_vec3_t *v, hx_vec3_t *result);

#endif /* !HX_VEC3_H */
