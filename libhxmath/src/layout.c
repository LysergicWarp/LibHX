#include <stddef.h>

#include <hx/types.h>

#include "layout.h"

/* layout.h against the types. The assembly reads some fields at layout.h's offsets and walks the
 * others in order, in the headers' inline assembly too, so every field's offset is checked.
 * Nothing here makes code */

#define FIELD(type, field, offset) \
        _Static_assert(offsetof(type, field) == (offset), #type "." #field " is not at " #offset)

#define SIZE(what, size) \
        _Static_assert(sizeof(what) == (size), #what " is not " #size " bytes")

/* The widths the assembly's word and long instructions take */
SIZE(hx_fix16_t, 4);
SIZE(hx_fix14_t, 2);
SIZE(hx_fix2_t, 2);

FIELD(hx_vec2_t, x, HX_VEC2_X);
FIELD(hx_vec2_t, y, HX_VEC2_Y);
SIZE(hx_vec2_t, HX_VEC2_SIZE);

FIELD(hx_vec3_t, x, HX_VEC3_X);
FIELD(hx_vec3_t, y, HX_VEC3_Y);
FIELD(hx_vec3_t, z, HX_VEC3_Z);
SIZE(hx_vec3_t, HX_VEC3_SIZE);

FIELD(hx_vtx_t, x, HX_VTX_X);
FIELD(hx_vtx_t, y, HX_VTX_Y);
FIELD(hx_vtx_t, z, HX_VTX_Z);
SIZE(hx_vtx_t, HX_VTX_SIZE);

FIELD(hx_screen_t, x, HX_SCREEN_X);
FIELD(hx_screen_t, y, HX_SCREEN_Y);
SIZE(hx_screen_t, HX_SCREEN_SIZE);

FIELD(hx_projected_t, screen, HX_PROJECTED_SCREEN);
FIELD(hx_projected_t, z, HX_PROJECTED_Z);
SIZE(hx_projected_t, HX_PROJECTED_SIZE);

FIELD(hx_mat_t, rotation, HX_MAT_ROTATION);
FIELD(hx_mat_t, unused, HX_MAT_UNUSED);
FIELD(hx_mat_t, translation, HX_MAT_TRANSLATION);
SIZE(hx_mat_t, HX_MAT_SIZE);
SIZE(((hx_mat_t *)0)->rotation, HX_MAT_ROTATION_SIZE);
SIZE(((hx_mat_t *)0)->rotation[0], HX_MAT_ROW_SIZE);
