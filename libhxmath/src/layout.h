#ifndef HX_SRC_LAYOUT_H
#define HX_SRC_LAYOUT_H

/* The public types as numbers, for the assembly: a field's offset and a type's size, in bytes.
 * layout.c checks each one against the type. Defines alone, so an assembly file can include it */

#define HX_VEC2_X               0
#define HX_VEC2_Y               4
#define HX_VEC2_SIZE            8

#define HX_VEC3_X               0
#define HX_VEC3_Y               4
#define HX_VEC3_Z               8
#define HX_VEC3_SIZE            12

#define HX_VTX_X                0
#define HX_VTX_Y                2
#define HX_VTX_Z                4
#define HX_VTX_SIZE             6

#define HX_SCREEN_X             0
#define HX_SCREEN_Y             2
#define HX_SCREEN_SIZE          4

#define HX_PROJECTED_SCREEN     0
#define HX_PROJECTED_Z          4
#define HX_PROJECTED_SIZE       8

#define HX_SINCOS_SINE          0
#define HX_SINCOS_COSINE        2
#define HX_SINCOS_SIZE          4

/* The rotation is three rows of three words */
#define HX_MAT_ROTATION         0
#define HX_MAT_ROW_SIZE         6
#define HX_MAT_ROTATION_SIZE    18
#define HX_MAT_UNUSED           18
#define HX_MAT_TRANSLATION      20
#define HX_MAT_SIZE             32

#endif /* !HX_SRC_LAYOUT_H */
