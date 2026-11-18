#ifndef HX_SRC_DIVIDE_H
#define HX_SRC_DIVIDE_H

#include <stdint.h>

#include <hx/fix16.h>

/* high:low over the divisor by the divide unit, rounded down. All three are taken as positive:
 * the divisor is below 2^31 and so is the quotient */
static inline uint32_t
hx_priv_divide(uint32_t high, uint32_t low, uint32_t divisor)
{
        volatile struct hx_priv_divider * const divider = HX_PRIV_DIVIDER;

        divider->dvsr = (int32_t)divisor;
        divider->dvdnth = (int32_t)high;
        divider->dvdntl = (int32_t)low;

        return (uint32_t)divider->dvdntl;
}

#endif /* !HX_SRC_DIVIDE_H */
