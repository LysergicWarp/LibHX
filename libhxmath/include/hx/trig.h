#ifndef HX_TRIG_H
#define HX_TRIG_H

#include <hx/types.h>

/* Sine and cosine, 4096 steps a turn */
extern const hx_fix14_t hx_priv_sincos[4096][2];

/* The tangent over half a turn, 1024 steps and the first again */
extern const hx_fix16_t hx_priv_tan[1025];

static inline hx_fix14_t
hx_sin(hx_angle_t angle)
{
        return hx_priv_sincos[(uint16_t)angle >> 4][0];
}

static inline hx_fix14_t
hx_cos(hx_angle_t angle)
{
        return hx_priv_sincos[(uint16_t)angle >> 4][1];
}

/* Between two table entries. From 256 units short of a quarter turn the error grows to an eighth
 * of the value. Within 32 units it is one straight line through the quarter turn, wrong in sign
 * for 19 units past it */
static inline hx_fix16_t
hx_tan(hx_angle_t angle)
{
        /* The entry by its byte offset, and the part between as the high word of a product */
        const hx_fix16_t * const entry =
            (const hx_fix16_t *)((const char *)hx_priv_tan + (((uint16_t)angle >> 3) & 0xFFCU));
        const hx_fix16_t below = entry[0];
        const int32_t part = (int32_t)(((uint32_t)angle & 31U) << 26);

        return below + (hx_fix16_t)(((int64_t)((entry[1] - below) * 2) * part) >> 32);
}

/* The angle of the direction (x, y) from +x, 0 for (0, 0) */
hx_angle_t hx_atan2(hx_fix16_t y, hx_fix16_t x);

#endif /* !HX_TRIG_H */
