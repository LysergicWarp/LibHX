#include <hx/cart.h>

#include <stddef.h>

#include "layout.h"

/* layout.h against the types: startup fills the record by layout.h's offsets */
#define FIELD(type, field, offset) \
        static_assert(offsetof(type, field) == (offset), #type "." #field " is not at " #offset)

#define SIZE(what, size) \
        static_assert(sizeof(what) == (size), #what " is not " #size " bytes")

FIELD(hx_cart_run_t, start, HX_CART_RUN_START);
FIELD(hx_cart_run_t, size, HX_CART_RUN_SIZE);
SIZE(hx_cart_run_t, HX_CART_RUN_T_SIZE);

FIELD(hx_cart_t, size, HX_CART_SIZE);
FIELD(hx_cart_t, id, HX_CART_ID);
FIELD(hx_cart_t, run_count, HX_CART_RUN_COUNT);
FIELD(hx_cart_t, runs, HX_CART_RUNS);
SIZE(hx_cart_t, HX_CART_T_SIZE);

/* What startup found. Zeroed with the rest, then filled, ahead of main */
extern hx_cart_t hx_priv_cart;
hx_cart_t hx_priv_cart;

const hx_cart_t *
hx_cart_found(void)
{
        return &hx_priv_cart;
}

void
hx_cart_require(uint32_t size, hx_cart_short_t *handler, void *context)
{
        if (hx_priv_cart.size >= size) {
                return;
        }
        handler(&hx_priv_cart, context);
        for (;;) {
        }
}
