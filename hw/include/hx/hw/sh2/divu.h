#ifndef HX_HW_SH2_DIVU_H
#define HX_HW_SH2_DIVU_H

#include <hx/hw/map.h>

/* The SH-2's divide unit: 64 bits by 32 or 32 by 32, signed, in 39 clocks.
 *
 * DVSR takes the divisor. A write to DVDNT starts 32 by 32, a write to DVDNTL starts 64 by 32
 * with DVDNTH written ahead of it. The quotient comes back where the write went, the remainder
 * in DVDNTH. A read while the unit works waits for it. The instruction after the one that starts
 * a divide writes no register of the unit.
 *
 * All are read and written as 32 bits. DVCR and VCRDIV take 16 too. */

#define SH2_DVSR 0xFFFFFF00
#define SH2_DVDNT 0xFFFFFF04
#define SH2_DVCR 0xFFFFFF08
#define SH2_VCRDIV 0xFFFFFF0C
#define SH2_DVDNTH 0xFFFFFF10
#define SH2_DVDNTL 0xFFFFFF14

/* The overflow interrupt's enable, and the overflow flag, which stays set until written 0 */
#define SH2_DVCR_OVFIE 0x00000002
#define SH2_DVCR_OVF 0x00000001

/* The overflow interrupt's vector number */
#define SH2_VCRDIV_VECTOR 0x0000007F

#ifndef __ASSEMBLER__

struct sh2_divu {
        int32_t dvsr;
        int32_t dvdnt;
        uint32_t dvcr;
        uint32_t vcrdiv;
        int32_t dvdnth;
        int32_t dvdntl;
};

#define SH2_DIVU ((volatile struct sh2_divu *)SH2_DVSR)

HX_MAP_AT(struct sh2_divu, dvsr, SH2_DVSR, SH2_DVSR);
HX_MAP_AT(struct sh2_divu, dvdnt, SH2_DVSR, SH2_DVDNT);
HX_MAP_AT(struct sh2_divu, dvcr, SH2_DVSR, SH2_DVCR);
HX_MAP_AT(struct sh2_divu, vcrdiv, SH2_DVSR, SH2_VCRDIV);
HX_MAP_AT(struct sh2_divu, dvdnth, SH2_DVSR, SH2_DVDNTH);
HX_MAP_AT(struct sh2_divu, dvdntl, SH2_DVSR, SH2_DVDNTL);

#endif /* !__ASSEMBLER__ */

#endif /* !HX_HW_SH2_DIVU_H */
