#include <hx/ref.h>
#include <hx/trig.h>

#define INLINE static inline __attribute__ ((always_inline))

/* Half of the last place kept, for the form that rounds to the nearest */
#define HALF 0x2000

/* A sum of 2.14 by 2.14 products as 2.14. The sums are unsigned so that they wrap */
INLINE hx_fix14_t
narrowed(uint32_t sum)
{
        return (hx_fix14_t)((sum << 2) >> 16);
}

INLINE uint32_t
product(int32_t a, int32_t b)
{
        return (uint32_t)(a * b);
}

/* A 2.14 row by a 16.16 vector as 16.16, on top of start */
INLINE hx_fix16_t
row_by_vector(int32_t r0, int32_t r1, int32_t r2, const hx_vec3_t *vector, int64_t start)
{
        const int64_t sum = start + ((int64_t)r0 * vector->x) + ((int64_t)r1 * vector->y) +
            ((int64_t)r2 * vector->z);
        const uint64_t up = (uint64_t)sum << 2;

        return (hx_fix16_t)(((uint32_t)(up >> 32) << 16) | ((uint32_t)up >> 16));
}

INLINE void
multiplied(const hx_mat_t *a, const hx_mat_t *b, hx_mat_t * restrict result, uint32_t start)
{
        for (uint32_t i = 0; i < 3U; i++) {
                const int32_t r0 = a->rotation[i][0];
                const int32_t r1 = a->rotation[i][1];
                const int32_t r2 = a->rotation[i][2];

                for (uint32_t j = 0; j < 3U; j++) {
                        result->rotation[i][j] = narrowed(start + product(r0, b->rotation[0][j]) +
                            product(r1, b->rotation[1][j]) + product(r2, b->rotation[2][j]));
                }
        }

        result->translation.x = (hx_fix16_t)((uint32_t)a->translation.x +
            (uint32_t)row_by_vector(a->rotation[0][0], a->rotation[0][1], a->rotation[0][2],
            &b->translation, start));
        result->translation.y = (hx_fix16_t)((uint32_t)a->translation.y +
            (uint32_t)row_by_vector(a->rotation[1][0], a->rotation[1][1], a->rotation[1][2],
            &b->translation, start));
        result->translation.z = (hx_fix16_t)((uint32_t)a->translation.z +
            (uint32_t)row_by_vector(a->rotation[2][0], a->rotation[2][1], a->rotation[2][2],
            &b->translation, start));
}

void
hx_ref_mat_mul(const hx_mat_t *a, const hx_mat_t *b, hx_mat_t *result)
{
        multiplied(a, b, result, 0);
}

void
hx_ref_mat_mul_round(const hx_mat_t *a, const hx_mat_t *b, hx_mat_t *result)
{
        multiplied(a, b, result, HALF);
}

void
hx_ref_mat_transpose(const hx_mat_t *m, hx_mat_t *result)
{
        const hx_mat_t from = *m;

        for (uint32_t i = 0; i < 3U; i++) {
                for (uint32_t j = 0; j < 3U; j++) {
                        result->rotation[i][j] = from.rotation[j][i];
                }
        }

        result->translation.x = 0;
        result->translation.y = 0;
        result->translation.z = 0;
}

void
hx_ref_mat_invert_rigid(const hx_mat_t *m, hx_mat_t *result)
{
        const hx_mat_t from = *m;

        for (uint32_t i = 0; i < 3U; i++) {
                for (uint32_t j = 0; j < 3U; j++) {
                        result->rotation[i][j] = from.rotation[j][i];
                }
        }

        /* The translation turned back: each of the rotation's columns by it, negated whole */
        result->translation.x = row_by_vector(-from.rotation[0][0], -from.rotation[1][0],
            -from.rotation[2][0], &from.translation, 0);
        result->translation.y = row_by_vector(-from.rotation[0][1], -from.rotation[1][1],
            -from.rotation[2][1], &from.translation, 0);
        result->translation.z = row_by_vector(-from.rotation[0][2], -from.rotation[1][2],
            -from.rotation[2][2], &from.translation, 0);
}

/* The turn takes the axis "from" toward the axis "toward" and keeps the third */
INLINE void
turned(const hx_mat_t *m, const hx_sincos_t *turn, hx_mat_t *result, uint32_t from,
    uint32_t toward, uint32_t kept)
{
        const int32_t sine = turn->sine;
        const int32_t cosine = turn->cosine;

        for (uint32_t i = 0; i < 3U; i++) {
                const int32_t a = m->rotation[i][from];
                const int32_t b = m->rotation[i][toward];

                result->rotation[i][kept] = m->rotation[i][kept];
                result->rotation[i][from] = narrowed(product(a, cosine) + product(b, sine));
                result->rotation[i][toward] = narrowed(product(b, cosine) - product(a, sine));
        }

        result->translation = m->translation;
}

void
hx_ref_mat_rotate_x_sincos(const hx_mat_t *m, const hx_sincos_t *turn, hx_mat_t *result)
{
        turned(m, turn, result, 1, 2, 0);
}

void
hx_ref_mat_rotate_y_sincos(const hx_mat_t *m, const hx_sincos_t *turn, hx_mat_t *result)
{
        turned(m, turn, result, 2, 0, 1);
}

void
hx_ref_mat_rotate_z_sincos(const hx_mat_t *m, const hx_sincos_t *turn, hx_mat_t *result)
{
        turned(m, turn, result, 0, 1, 2);
}

void
hx_ref_mat_rotate_x(const hx_mat_t *m, hx_angle_t angle, hx_mat_t *result)
{
        const hx_sincos_t turn = { hx_sin(angle), hx_cos(angle) };

        turned(m, &turn, result, 1, 2, 0);
}

void
hx_ref_mat_rotate_y(const hx_mat_t *m, hx_angle_t angle, hx_mat_t *result)
{
        const hx_sincos_t turn = { hx_sin(angle), hx_cos(angle) };

        turned(m, &turn, result, 2, 0, 1);
}

void
hx_ref_mat_rotate_z(const hx_mat_t *m, hx_angle_t angle, hx_mat_t *result)
{
        const hx_sincos_t turn = { hx_sin(angle), hx_cos(angle) };

        turned(m, &turn, result, 0, 1, 2);
}
