#ifndef HX_SRC_TABLES_H
#define HX_SRC_TABLES_H

#include <stdint.h>

/* The arctangent of 0 to 1 in 1024 steps, as an angle */
extern const uint16_t hx_priv_arcs[1025];

/* One over the root of 2^30 to 2^32 by its top 8 bits less 64, as a share of 65536 rounded down
 * and held under it, and the step down to the next */
extern const uint16_t hx_priv_reciprocal_roots[192][2];

#endif /* !HX_SRC_TABLES_H */
