#ifndef HX_TRIG_H
#define HX_TRIG_H

#include <hx/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Sine and cosine, 4096 steps a turn */
extern const hx_fix14_t hx_priv_sincos[4096][2];

/* The tangent over half a turn, 1024 steps and the first again */
extern const hx_fix16_t hx_priv_tan[1025];

/* An angle unit in radians, by 2^32 */
#define HX_PRIV_ANGLE_UNIT 411774

/* The sine to the nearest last place, one off at most: the table's nearest entry, and from there
 * along its slope, which is its cosine */
static inline hx_fix14_t
hx_sin(hx_angle_t angle)
{
        const uint32_t turned = (uint32_t)(uint16_t)angle + 8U;
        const int32_t apart = (int32_t)(turned & 15U) - 8;
        const hx_fix14_t * const pair = hx_priv_sincos[(turned >> 4) & 4095U];
        const int64_t along = ((int64_t)(apart * pair[1]) * HX_PRIV_ANGLE_UNIT) + 0x80000000LL;

        return (hx_fix14_t)(pair[0] + (int32_t)(along >> 32));
}

/* The cosine, as the sine: its slope is the sine negated */
static inline hx_fix14_t
hx_cos(hx_angle_t angle)
{
        const uint32_t turned = (uint32_t)(uint16_t)angle + 8U;
        const int32_t apart = (int32_t)(turned & 15U) - 8;
        const hx_fix14_t * const pair = hx_priv_sincos[(turned >> 4) & 4095U];
        const int64_t along = ((int64_t)(apart * pair[0]) * HX_PRIV_ANGLE_UNIT) + 0x80000000LL;

        return (hx_fix14_t)(pair[1] - (int32_t)(along >> 32));
}

/* The sine and the cosine of the angle's top 12 bits: the table's entry at or below the angle.
 * Up to 25 last places off the angle's own */
static inline hx_fix14_t
hx_sin_fast(hx_angle_t angle)
{
        return hx_priv_sincos[(uint16_t)angle >> 4][0];
}

static inline hx_fix14_t
hx_cos_fast(hx_angle_t angle)
{
        return hx_priv_sincos[(uint16_t)angle >> 4][1];
}

/* The tangent to the nearest 1/65536, one off at most. The largest value there is for a quarter
 * turn and for three quarters, where it has none. Uses the divide unit */
hx_fix16_t hx_tan(hx_angle_t angle);

/* Between two table entries. From 256 units short of a quarter turn the error grows to an eighth
 * of the value. Within 32 units it is one straight line through the quarter turn, wrong in sign
 * for 19 units past it */
static inline hx_fix16_t
hx_tan_fast(hx_angle_t angle)
{
        /* The entry by its byte offset, and the part between as the high word of a product */
        const hx_fix16_t * const entry =
            (const hx_fix16_t *)((const char *)hx_priv_tan + (((uint16_t)angle >> 3) & 0xFFCU));
        const hx_fix16_t below = entry[0];
        const int32_t part = (int32_t)(((uint32_t)angle & 31U) << 26);

        return below + (hx_fix16_t)(((int64_t)((entry[1] - below) * 2) * part) >> 32);
}

/* The angle of the direction (x, y) from +x to the nearest unit, one off at most. 0 for (0, 0).
 * Uses the divide unit */
hx_angle_t hx_atan2(hx_fix16_t y, hx_fix16_t x);

/* The same by the nearest of 1024 steps of the smaller coordinate over the larger, up to 6 units
 * off */
hx_angle_t hx_atan2_fast(hx_fix16_t y, hx_fix16_t x);

#ifdef __cplusplus
}
#endif

#endif /* !HX_TRIG_H */
