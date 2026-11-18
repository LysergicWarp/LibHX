#ifndef HX_MAT_H
#define HX_MAT_H

#include <hx/types.h>
#include <hx/fix16.h>
#include <hx/trig.h>

/* A function with one matrix to read may write its result over it, whole. hx_mat_mul may not. */

void hx_mat_identity(hx_mat_t *result);

/* a by b: b is applied to the point first. The result is neither a nor b */
void hx_mat_mul(const hx_mat_t *a, const hx_mat_t *b, hx_mat_t *result);
void hx_mat_mul_round(const hx_mat_t *a, const hx_mat_t *b, hx_mat_t *result);

/* The rotation transposed, no translation */
void hx_mat_transpose(const hx_mat_t *m, hx_mat_t *result);

/* The inverse of a rotation with a translation. Not for a matrix that scales */
void hx_mat_invert_rigid(const hx_mat_t *m, hx_mat_t *result);

/* m by a turn about an axis: the turn is applied to the point first */
void hx_mat_rotate_x(const hx_mat_t *m, hx_angle_t angle, hx_mat_t *result);
void hx_mat_rotate_y(const hx_mat_t *m, hx_angle_t angle, hx_mat_t *result);
void hx_mat_rotate_z(const hx_mat_t *m, hx_angle_t angle, hx_mat_t *result);

/* A turn about an axis, no translation */
void hx_mat_rotation_x(hx_angle_t angle, hx_mat_t *result);
void hx_mat_rotation_y(hx_angle_t angle, hx_mat_t *result);
void hx_mat_rotation_z(hx_angle_t angle, hx_mat_t *result);

/* The view of a camera at eye that looks at target: target comes out on +z, eye at (0, 0, 0).
 * The camera stays level, its x along the ground, then turns about its line of sight by roll,
 * its x toward its y. The picture turns the other way: the view is hx_mat_rotation_z of the roll
 * negated, applied behind the level view.
 *
 * With no roll each rotation word is within 1/16384 of the exact one. Straight along y the camera
 * has no level: it is then taken as facing +z ahead of its tilt. With target at eye it faces +z.
 * The two are under 32768 apart on every axis: past that the difference does not fit and the
 * view faces elsewhere */
void hx_mat_lookat(const hx_vec3_t *eye, const hx_vec3_t *target, hx_angle_t roll,
    hx_mat_t *result);

/* v by m */
void hx_mat_transform(const hx_mat_t *m, const hx_vtx_t *v, hx_vec3_t *result);

/* v by m's rotation alone, no translation: a direction or a normal. Each coordinate is three
 * products summed whole, then rounded down once, and it wraps. The result may be v */
void hx_mat_transform_vector(const hx_mat_t *m, const hx_vec3_t *v, hx_vec3_t *result);

/* v by m and onto a screen at that distance: x and y in pixels from the screen's centre. Returns
 * z as hx_mat_transform gives it.
 *
 * A pixel is the coordinate by distance / z, the quotient cut off as 16.16, rounded down: within
 * one of the exact pixel rounded down, on either side.
 *
 * z is above distance / 32768, and each of x and y fits 16 bits: a point too near or far off the
 * screen is the caller's to drop ahead of the call. Uses the divide unit */
hx_fix16_t hx_mat_project(const hx_mat_t *m, const hx_vtx_t *v, hx_fix16_t distance,
    hx_screen_t *result);

/* The batch forms: count points of source, one after another, into as many of result. Each
 * comes out as its single form gives it. A count of 0 does nothing. */

void hx_mat_transform_batch(const hx_mat_t *m, const hx_vtx_t *source, uint32_t count,
    hx_vec3_t *result);

/* A point that hx_mat_project is not for has its z right and an x and y that mean nothing, and
 * the points around it come out as ever.
 *
 * One too near, z at or under distance / 32768, the caller drops by its z behind the call.
 *
 * One too far off the screen cannot be told from its result: its pixel is cut to 16 bits and
 * its z is as any other. With z at n or more a pixel fits while the view-space coordinate is
 * under n * 32768 / distance, so such a point is kept out ahead of the call.
 *
 * Uses the divide unit, whose overflow interrupt is taken as off, as a reset leaves it: a
 * point too near overflows it */
void hx_mat_project_batch(const hx_mat_t *m, const hx_vtx_t *source, uint32_t count,
    hx_fix16_t distance, hx_projected_t *result);

/* The distance of a screen that many pixels wide that fills that angle of view, for
 * hx_mat_project. The angle is under half a turn and wide enough for the distance to stay under
 * 32768: 114 or more for a screen 352 wide. A narrower one, 0 and 1 among them, gives what the
 * divide unit gives. The distance is as exact as hx_tan of half the angle. Uses the divide unit */
static inline hx_fix16_t
hx_view_distance(int32_t width, hx_angle_t angle)
{
        const hx_fix16_t half = (hx_fix16_t)((uint32_t)width << 15);

        return hx_div(half, hx_tan((hx_angle_t)((uint16_t)angle >> 1)));
}

#endif /* !HX_MAT_H */
