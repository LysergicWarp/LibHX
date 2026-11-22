#ifndef HX_SRC_LAYOUT_H
#define HX_SRC_LAYOUT_H

/* The public types as numbers, for the assembly: a field's offset and a type's size, in bytes.
 * cart.c checks each one against the type. Defines alone, so an assembly file can include it.
 *
 * Both types have a field named size, so here a name ending in _SIZE is that field's offset,
 * and a type's own size ends in _T_SIZE */

#define HX_CART_RUN_START       0
#define HX_CART_RUN_SIZE        4
#define HX_CART_RUN_T_SIZE      8

#define HX_CART_SIZE            0
#define HX_CART_ID              4
#define HX_CART_RUN_COUNT       5
#define HX_CART_RUNS            8
#define HX_CART_T_SIZE          24

#endif /* !HX_SRC_LAYOUT_H */
