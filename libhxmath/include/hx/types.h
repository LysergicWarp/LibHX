#ifndef HX_TYPES_H
#define HX_TYPES_H

#include <stdint.h>

/** @file
 * The math types, their literals, and the conversions between them. */

#ifdef __cplusplus
extern "C" {
#endif

/** @name Fixed point
 * The number in a name is its fraction bits */
/** @{ */
typedef int32_t hx_fix16_t; /**< 16.16 */
typedef int16_t hx_fix14_t; /**< 2.14 */
typedef int16_t hx_fix2_t;  /**< 14.2 */
/** @} */

/** 65536 a turn */
typedef int16_t hx_angle_t;

/* The assembly is written to the layouts of the types below. The library's build checks them
 * against src/layout.h, and a type changed here takes its functions with it */

typedef struct {
        hx_fix16_t x;
        hx_fix16_t y;
} hx_vec2_t;

typedef struct {
        hx_fix16_t x;
        hx_fix16_t y;
        hx_fix16_t z;
} hx_vec3_t;

/** A model's point, 6 bytes */
typedef struct {
        hx_fix2_t x;
        hx_fix2_t y;
        hx_fix2_t z;
} hx_vtx_t;

/** A point of the screen in pixels from its centre */
typedef struct {
        int16_t x;
        int16_t y;
} hx_screen_t;

/** A point of the screen and its z in view space, 8 bytes */
typedef struct {
        hx_screen_t screen;
        hx_fix16_t z;
} hx_projected_t;

/** A rotation by rows and a translation, 32 bytes. The functions leave the unused word alone */
typedef struct {
        hx_fix14_t rotation[3][3];
        int16_t unused;
        hx_vec3_t translation;
} hx_mat_t;

#define HX_PRIV_SCALED(x, one) (((x) >= 0) ? (((x) * (one)) + 0.5) : (((x) * (one)) - 0.5))

#define HX_FIX16(x) ((hx_fix16_t)HX_PRIV_SCALED(x, 65536.0))
#define HX_FIX14(x) ((hx_fix14_t)HX_PRIV_SCALED(x, 16384.0))
#define HX_FIX2(x)  ((hx_fix2_t)HX_PRIV_SCALED(x, 4.0))
#define HX_DEG(x)   ((hx_angle_t)(uint16_t)(int32_t)HX_PRIV_SCALED(x, 65536.0 / 360.0))

/** @name Conversions
 * hx_T_from_x makes a T and hx_T_to_x leaves one. A plain one that loses places rounds down,
 * and a value past the result's range wraps */
/** @{ */

static inline hx_fix16_t
hx_fix16_from_int(int32_t value)
{
        return (hx_fix16_t)((uint32_t)value << 16);
}

static inline int32_t
hx_fix16_to_int(hx_fix16_t value)
{
        return value >> 16;
}

/** To the nearest, a half goes up */
static inline int32_t
hx_fix16_to_int_round(hx_fix16_t value)
{
        return (value >> 16) + (int32_t)(((uint32_t)value >> 15) & 1U);
}

static inline hx_fix16_t
hx_fix16_from_fix14(hx_fix14_t value)
{
        return (hx_fix16_t)((uint32_t)(int32_t)value << 2);
}

/** Holds from -2 up to 2 */
static inline hx_fix14_t
hx_fix16_to_fix14(hx_fix16_t value)
{
        return (hx_fix14_t)((uint32_t)value >> 2);
}

static inline hx_fix16_t
hx_fix16_from_fix2(hx_fix2_t value)
{
        return (hx_fix16_t)((uint32_t)(int32_t)value << 14);
}

/** Holds from -8192 up to 8192 */
static inline hx_fix2_t
hx_fix16_to_fix2(hx_fix16_t value)
{
        return (hx_fix2_t)(((uint32_t)value << 2) >> 16);
}

static inline hx_fix2_t
hx_fix2_from_int(int32_t value)
{
        return (hx_fix2_t)((uint32_t)value << 2);
}

static inline int32_t
hx_fix2_to_int(hx_fix2_t value)
{
        return value >> 2;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* !HX_TYPES_H */
