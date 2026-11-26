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

void
hx_mat_zero(hx_mat_t *result)
{
        result->rotation[0][0] = 0;
        result->rotation[0][1] = 0;
        result->rotation[0][2] = 0;
        result->rotation[1][0] = 0;
        result->rotation[1][1] = 0;
        result->rotation[1][2] = 0;
        result->rotation[2][0] = 0;
        result->rotation[2][1] = 0;
        result->rotation[2][2] = 0;
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

/* A sum of 2.14 by 2.14 products as 2.14, rounded down. The sums are unsigned so that they wrap */
static inline __attribute__ ((always_inline)) hx_fix14_t
narrowed(uint32_t sum)
{
        return (hx_fix14_t)((sum << 2) >> 16);
}

static inline __attribute__ ((always_inline)) uint32_t
product(int32_t a, int32_t b)
{
        return (uint32_t)(a * b);
}

/* The turn about y, turned about x and then about z as hx_mat_rotate_x_sincos and its z form
 * turn it. A product with a 0 or a 1 in it is left out, which changes no word of a pair within
 * -1 to 1, as hx_sin's, hx_cos's and the table's are. A y sine of -2 has no negative in 2.14:
 * there the words part from the three turns' */
static inline __attribute__ ((always_inline)) void
turned_yxz(const hx_sincos_t *y, const hx_sincos_t *x, const hx_sincos_t *z, hx_mat_t *result)
{
        const int32_t sine_y = y->sine;
        const int32_t cosine_y = y->cosine;
        const int32_t sine_x = x->sine;
        const int32_t cosine_x = x->cosine;
        const int32_t sine_z = z->sine;
        const int32_t cosine_z = z->cosine;

        /* The y words of rows x and z once turned about x, which the turn about z takes on */
        const int32_t up_x = narrowed(product(sine_y, sine_x));
        const int32_t up_z = narrowed(product(cosine_y, sine_x));

        result->rotation[0][0] = narrowed(product(cosine_y, cosine_z) + product(up_x, sine_z));
        result->rotation[0][1] = narrowed(product(up_x, cosine_z) - product(cosine_y, sine_z));
        result->rotation[0][2] = narrowed(product(sine_y, cosine_x));
        result->rotation[1][0] = narrowed(product(cosine_x, sine_z));
        result->rotation[1][1] = narrowed(product(cosine_x, cosine_z));
        result->rotation[1][2] = (hx_fix14_t)-sine_x;
        result->rotation[2][0] = narrowed(product(up_z, sine_z) - product(sine_y, cosine_z));
        result->rotation[2][1] = narrowed(product(up_z, cosine_z) + product(sine_y, sine_z));
        result->rotation[2][2] = narrowed(product(cosine_y, cosine_x));
        result->translation.x = 0;
        result->translation.y = 0;
        result->translation.z = 0;
}

void
hx_mat_rotation_yxz(hx_angle_t y, hx_angle_t x, hx_angle_t z, hx_mat_t *result)
{
        hx_sincos_t turn_y;
        hx_sincos_t turn_x;
        hx_sincos_t turn_z;

        hx_sincos(y, &turn_y);
        hx_sincos(x, &turn_x);
        hx_sincos(z, &turn_z);
        turned_yxz(&turn_y, &turn_x, &turn_z, result);
}

void
hx_mat_rotation_yxz_fast(hx_angle_t y, hx_angle_t x, hx_angle_t z, hx_mat_t *result)
{
        turned_yxz(&hx_priv_sincos[(uint16_t)y >> 4], &hx_priv_sincos[(uint16_t)x >> 4],
            &hx_priv_sincos[(uint16_t)z >> 4], result);
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
hx_mat_translate(const hx_mat_t *m, const hx_vec3_t *v, hx_mat_t *result)
{
        const int32_t x = v->x;
        const int32_t y = v->y;
        const int32_t z = v->z;

        /* Everything is read ahead of the first write: the result may be m, and v may lie in it */
        const uint32_t moved_x = (uint32_t)m->translation.x +
            (uint32_t)row_by(m->rotation[0], x, y, z);
        const uint32_t moved_y = (uint32_t)m->translation.y +
            (uint32_t)row_by(m->rotation[1], x, y, z);
        const uint32_t moved_z = (uint32_t)m->translation.z +
            (uint32_t)row_by(m->rotation[2], x, y, z);

        result->rotation[0][0] = m->rotation[0][0];
        result->rotation[0][1] = m->rotation[0][1];
        result->rotation[0][2] = m->rotation[0][2];
        result->rotation[1][0] = m->rotation[1][0];
        result->rotation[1][1] = m->rotation[1][1];
        result->rotation[1][2] = m->rotation[1][2];
        result->rotation[2][0] = m->rotation[2][0];
        result->rotation[2][1] = m->rotation[2][1];
        result->rotation[2][2] = m->rotation[2][2];
        result->translation.x = (hx_fix16_t)moved_x;
        result->translation.y = (hx_fix16_t)moved_y;
        result->translation.z = (hx_fix16_t)moved_z;
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
