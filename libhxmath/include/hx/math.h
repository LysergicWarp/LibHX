/* libhxmath: fixed-point math for the SH-2.
 *
 * Conventions, the same in every function:
 * - x right, y down, z into the screen.
 * - An angle is 65536 a turn. A positive one turns x toward y, y toward z, z toward x.
 * - A matrix is applied as a row times the point, plus the translation.
 * - The result is the last argument. An aggregate comes back through it, a scalar by value.
 * - The plain name neither rounds to the nearest nor saturates. A form that does says so in its
 *   name. The sine, cosine, tangent and atan2 are the exception: they are to the nearest under
 *   their plain names, and the tangent gives its largest value where it has none.
 * - The plain name is accurate. A _fast form gives up accuracy for speed, by its name.
 * - SR's S bit is clear when a function is called. MACH and MACL are not kept. */

#ifndef HX_MATH_H
#define HX_MATH_H

#include <hx/types.h>
#include <hx/fix16.h>
#include <hx/trig.h>
#include <hx/vec2.h>
#include <hx/vec3.h>
#include <hx/mat.h>

#endif /* !HX_MATH_H */
