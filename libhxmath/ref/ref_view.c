#include <hx/ref.h>
#include <hx/fix16.h>

#define INLINE static inline __attribute__ ((always_inline))

/* A 2.14 row by a 14.2 point is 16.16 as it stands. Unsigned so that the sum wraps */
INLINE hx_fix16_t
row_by_point(const hx_fix14_t *row, hx_fix16_t translation, int32_t x, int32_t y, int32_t z)
{
        return (hx_fix16_t)((uint32_t)translation + (uint32_t)(row[0] * x) + (uint32_t)(row[1] * y) +
            (uint32_t)(row[2] * z));
}

void
hx_ref_mat_transform(const hx_mat_t *m, const hx_vtx_t *v, hx_vec3_t *result)
{
        const int32_t x = v->x;
        const int32_t y = v->y;
        const int32_t z = v->z;

        result->x = row_by_point(m->rotation[0], m->translation.x, x, y, z);
        result->y = row_by_point(m->rotation[1], m->translation.y, x, y, z);
        result->z = row_by_point(m->rotation[2], m->translation.z, x, y, z);
}

hx_fix16_t
hx_ref_mat_project(const hx_mat_t *m, const hx_vtx_t *v, hx_fix16_t distance, hx_screen_t *result)
{
        const int32_t x = v->x;
        const int32_t y = v->y;
        const int32_t z = v->z;

        /* z first: the divide runs under the other two rows */
        const hx_fix16_t depth = row_by_point(m->rotation[2], m->translation.z, x, y, z);

        hx_div_start(distance, depth);

        const hx_fix16_t across = row_by_point(m->rotation[0], m->translation.x, x, y, z);
        const hx_fix16_t down = row_by_point(m->rotation[1], m->translation.y, x, y, z);
        const hx_fix16_t scale = hx_div_collect();

        result->x = (int16_t)(((int64_t)across * scale) >> 32);
        result->y = (int16_t)(((int64_t)down * scale) >> 32);

        return depth;
}

void
hx_ref_mat_transform_batch(const hx_mat_t *m, const hx_vtx_t *source, uint32_t count,
    hx_vec3_t *result)
{
        for (uint32_t i = 0; i < count; i++) {
                hx_ref_mat_transform(m, &source[i], &result[i]);
        }
}

/* The assembly starts a point's divide ahead of the point before it being finished. The results
 * are the same */
void
hx_ref_mat_project_batch(const hx_mat_t *m, const hx_vtx_t *source, uint32_t count,
    hx_fix16_t distance, hx_projected_t *result)
{
        for (uint32_t i = 0; i < count; i++) {
                result[i].z = hx_ref_mat_project(m, &source[i], distance, &result[i].screen);
        }
}
