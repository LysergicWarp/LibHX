#ifndef HX_REF_H
#define HX_REF_H

#include <hx/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The assembly functions in C: the same results, to check them against and to read */

hx_fix16_t hx_ref_sqrt(hx_fix16_t value);
hx_fix16_t hx_ref_rsqrt(hx_fix16_t value);

hx_angle_t hx_ref_atan2_fast(hx_fix16_t y, hx_fix16_t x);

hx_fix16_t hx_ref_vec3_dot(const hx_vec3_t *a, const hx_vec3_t *b);
void hx_ref_vec3_cross(const hx_vec3_t *a, const hx_vec3_t *b, hx_vec3_t *result);
void hx_ref_vec3_normalize(const hx_vec3_t *v, hx_vec3_t *result);
void hx_ref_vec3_normalize_round(const hx_vec3_t *v, hx_vec3_t *result);

void hx_ref_mat_mul(const hx_mat_t *a, const hx_mat_t *b, hx_mat_t *result);
void hx_ref_mat_mul_round(const hx_mat_t *a, const hx_mat_t *b, hx_mat_t *result);
void hx_ref_mat_transpose(const hx_mat_t *m, hx_mat_t *result);
void hx_ref_mat_invert_rigid(const hx_mat_t *m, hx_mat_t *result);
void hx_ref_mat_rotate_x(const hx_mat_t *m, hx_angle_t angle, hx_mat_t *result);
void hx_ref_mat_rotate_y(const hx_mat_t *m, hx_angle_t angle, hx_mat_t *result);
void hx_ref_mat_rotate_z(const hx_mat_t *m, hx_angle_t angle, hx_mat_t *result);
void hx_ref_mat_lookat(const hx_vec3_t *eye, const hx_vec3_t *target, hx_angle_t roll,
    hx_mat_t *result);
void hx_ref_mat_transform(const hx_mat_t *m, const hx_vtx_t *v, hx_vec3_t *result);
hx_fix16_t hx_ref_mat_project(const hx_mat_t *m, const hx_vtx_t *v, hx_fix16_t distance,
    hx_screen_t *result);
void hx_ref_mat_transform_batch(const hx_mat_t *m, const hx_vtx_t *source, uint32_t count,
    hx_vec3_t *result);
void hx_ref_mat_project_batch(const hx_mat_t *m, const hx_vtx_t *source, uint32_t count,
    hx_fix16_t distance, hx_projected_t *result);

#ifdef __cplusplus
}
#endif

#endif /* !HX_REF_H */
