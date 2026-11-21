#ifndef HX_HW_MAP_H
#define HX_HW_MAP_H

/** @file
 * A register map names each register's address as a number, which assembly takes too, and lays a
 * struct over the module for C. A register is reached at its field's width alone. Which way it
 * goes, read or written, is in its map's comments.
 *
 * A bit is named by its register: the mask of one bit, or of a whole field of several. */

#ifndef __ASSEMBLER__

#include <stddef.h>
#include <stdint.h>

/* The struct's field is where the register's address says, counted from the module's first */
#ifdef __cplusplus
#define HX_MAP_AT(type, field, first, address) \
        static_assert(offsetof(type, field) == (address) - (first), #address)
#else
#define HX_MAP_AT(type, field, first, address) \
        _Static_assert(offsetof(type, field) == (address) - (first), #address)
#endif

#endif /* !__ASSEMBLER__ */

#endif /* !HX_HW_MAP_H */
