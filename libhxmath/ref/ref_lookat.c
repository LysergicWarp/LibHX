#include <hx/mat.h>
#include <hx/ref.h>

#include "length.h"

/* One as 2.30, the format the camera's sines and cosines are worked in */
#define ONE 0x40000000

INLINE int32_t
signed_as(uint32_t value, int32_t sign)
{
        return (sign < 0) ? -(int32_t)value : (int32_t)value;
}

/* Down by a count of 31 at most, in the same few steps for any count */
INLINE uint32_t
lowered_by(uint32_t value, uint32_t count)
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

/* 2.30 by 2.30 */
INLINE int32_t
product(int32_t a, int32_t b)
{
        const int64_t whole = (int64_t)a * b;

        return (int32_t)(((uint32_t)(whole >> 32) << 2) | ((uint32_t)whole >> 30));
}

/* 2.30 as 2.14, to the nearest */
INLINE hx_fix14_t
narrowed(int32_t value)
{
        return (hx_fix14_t)((value + 0x8000) >> 16);
}

/* a by the cosine plus b by the sine, 2.30 by 2.14, as 2.14 to the nearest */
INLINE hx_fix14_t
turned(int32_t a, int32_t cosine, int32_t b, int32_t sine)
{
        const uint64_t sum = (uint64_t)((int64_t)a * cosine) + (uint64_t)((int64_t)b * sine) +
            0x20000000U;

        return (hx_fix14_t)(((uint32_t)(sum >> 32) << 2) | ((uint32_t)sum >> 30));
}

/* A 2.14 row by a 16.16 point as 16.16, negated whole and rounded down once */
INLINE hx_fix16_t
back_along(const hx_fix14_t *row, int32_t x, int32_t y, int32_t z)
{
        const uint64_t sum = 0U - ((uint64_t)((int64_t)row[0] * x) + (uint64_t)((int64_t)row[1] * y) +
            (uint64_t)((int64_t)row[2] * z));

        return (hx_fix16_t)(((uint32_t)(sum >> 32) << 18) | ((uint32_t)sum >> 14));
}

void
hx_ref_mat_lookat(const hx_vec3_t *eye, const hx_vec3_t *target, hx_angle_t roll,
    hx_mat_t *result)
{
        const int32_t eye_x = eye->x;
        const int32_t eye_y = eye->y;
        const int32_t eye_z = eye->z;
        const int32_t x = (int32_t)((uint32_t)target->x - (uint32_t)eye_x);
        const int32_t y = (int32_t)((uint32_t)target->y - (uint32_t)eye_y);
        const int32_t z = (int32_t)((uint32_t)target->z - (uint32_t)eye_z);
        uint32_t ux = magnitude(x);
        uint32_t uy = magnitude(y);
        uint32_t uz = magnitude(z);

        /* The camera's turn about y and its tilt out of the level, sizes alone. With nothing to
         * go by it faces +z, level */
        uint32_t turn_sine = 0;
        uint32_t turn_cosine = ONE;
        uint32_t tilt_sine = 0;
        uint32_t tilt_cosine = ONE;

        if ((ux | uy | uz) != 0) {
                /* The level part is brought up by itself, or a steep view has few bits of it left
                 * to turn by. It starts from the values as given: what the whole loses on its
                 * way down is not lost to it */
                uint32_t px = ux;
                uint32_t pz = uz;
                uint32_t none = 0;

                const int32_t whole_up = raised(&ux, &uy, &uz);

                if ((px | pz) == 0) {
                        tilt_sine = ONE;
                        tilt_cosine = 0;
                } else {
                        /* How far the level part ends above the whole, 31 at most */
                        const uint32_t above = (uint32_t)(raised(&px, &pz, &none) - whole_up);
                        const struct length flat = length_of(squares_high(px, pz, 0));
                        const struct length whole = length_of(squares_high(ux, uy, uz));

                        /* A size by a reciprocal is their quotient by 2^60 */
                        turn_sine = high_product(px, flat.reciprocal) << 2;
                        turn_cosine = high_product(pz, flat.reciprocal) << 2;
                        tilt_sine = high_product(uy, whole.reciprocal) << 2;
                        tilt_cosine = lowered_by(high_product(flat.twice, whole.reciprocal) << 1,
                            above);
                }
        }

        const int32_t sy = signed_as(turn_sine, x);
        const int32_t cy = signed_as(turn_cosine, z);
        const int32_t st = signed_as(tilt_sine, y);
        const int32_t ct = (int32_t)tilt_cosine;

        /* The camera's x and y in the world, level */
        const int32_t across[3] = { cy, 0, -sy };
        const int32_t down[3] = { -product(sy, st), ct, -product(cy, st) };

        if (roll == 0) {
                for (uint32_t i = 0; i < 3U; i++) {
                        result->rotation[0][i] = narrowed(across[i]);
                        result->rotation[1][i] = narrowed(down[i]);
                }
        } else {
                /* The picture's turn, the other way: the table's entry hx_mat_rotation_z reads for
                 * it, so the two agree at every angle */
                const hx_fix14_t * const pair = hx_priv_sincos[(uint16_t)-roll >> 4];
                const int32_t sine = pair[0];
                const int32_t cosine = pair[1];

                for (uint32_t i = 0; i < 3U; i++) {
                        result->rotation[0][i] = turned(across[i], cosine, down[i], -sine);
                        result->rotation[1][i] = turned(down[i], cosine, across[i], sine);
                }
        }

        result->rotation[2][0] = narrowed(product(sy, ct));
        result->rotation[2][1] = narrowed(st);
        result->rotation[2][2] = narrowed(product(cy, ct));

        result->translation.x = back_along(result->rotation[0], eye_x, eye_y, eye_z);
        result->translation.y = back_along(result->rotation[1], eye_x, eye_y, eye_z);
        result->translation.z = back_along(result->rotation[2], eye_x, eye_y, eye_z);
}
