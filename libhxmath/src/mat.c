#include <hx/mat.h>
#include <hx/trig.h>

void
hx_mat_identity(hx_mat_t *result)
{
        result->rotation[0][0] = HX_FIX14(1.0);
        result->rotation[0][1] = 0;
        result->rotation[0][2] = 0;
        result->rotation[1][0] = 0;
        result->rotation[1][1] = HX_FIX14(1.0);
        result->rotation[1][2] = 0;
        result->rotation[2][0] = 0;
        result->rotation[2][1] = 0;
        result->rotation[2][2] = HX_FIX14(1.0);
        result->translation.x = 0;
        result->translation.y = 0;
        result->translation.z = 0;
}

/* The turn takes the axis "from" toward the axis "toward" and keeps the third */
static inline __attribute__ ((always_inline)) void
turn(hx_fix14_t sine, hx_fix14_t cosine, hx_mat_t *result, uint32_t from, uint32_t toward,
    uint32_t kept)
{
        result->rotation[kept][kept] = HX_FIX14(1.0);
        result->rotation[kept][from] = 0;
        result->rotation[kept][toward] = 0;
        result->rotation[from][kept] = 0;
        result->rotation[from][from] = cosine;
        result->rotation[from][toward] = (hx_fix14_t)-sine;
        result->rotation[toward][kept] = 0;
        result->rotation[toward][from] = sine;
        result->rotation[toward][toward] = cosine;
        result->translation.x = 0;
        result->translation.y = 0;
        result->translation.z = 0;
}

void
hx_mat_rotation_x(hx_angle_t angle, hx_mat_t *result)
{
        turn(hx_sin(angle), hx_cos(angle), result, 1, 2, 0);
}

void
hx_mat_rotation_y(hx_angle_t angle, hx_mat_t *result)
{
        turn(hx_sin(angle), hx_cos(angle), result, 2, 0, 1);
}

void
hx_mat_rotation_z(hx_angle_t angle, hx_mat_t *result)
{
        turn(hx_sin(angle), hx_cos(angle), result, 0, 1, 2);
}

void
hx_mat_rotation_x_fast(hx_angle_t angle, hx_mat_t *result)
{
        turn(hx_sin_fast(angle), hx_cos_fast(angle), result, 1, 2, 0);
}

void
hx_mat_rotation_y_fast(hx_angle_t angle, hx_mat_t *result)
{
        turn(hx_sin_fast(angle), hx_cos_fast(angle), result, 2, 0, 1);
}

void
hx_mat_rotation_z_fast(hx_angle_t angle, hx_mat_t *result)
{
        turn(hx_sin_fast(angle), hx_cos_fast(angle), result, 0, 1, 2);
}

/* A row of the rotation by (x, y, z): three products summed whole, then rounded down once to
 * 16.16. Unsigned so that it wraps */
static inline __attribute__ ((always_inline)) hx_fix16_t
row_by(const hx_fix14_t *row, int32_t x, int32_t y, int32_t z)
{
        const uint64_t sum = (uint64_t)((int64_t)row[0] * x) + (uint64_t)((int64_t)row[1] * y) +
            (uint64_t)((int64_t)row[2] * z);

        return (hx_fix16_t)(((uint32_t)(sum >> 32) << 18) | ((uint32_t)sum >> 14));
}

void
hx_mat_transform_vector(const hx_mat_t *m, const hx_vec3_t *v, hx_vec3_t *result)
{
        const int32_t x = v->x;
        const int32_t y = v->y;
        const int32_t z = v->z;

        result->x = row_by(m->rotation[0], x, y, z);
        result->y = row_by(m->rotation[1], x, y, z);
        result->z = row_by(m->rotation[2], x, y, z);
}
