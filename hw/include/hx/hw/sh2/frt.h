#ifndef HX_HW_SH2_FRT_H
#define HX_HW_SH2_FRT_H

#include <hx/hw/map.h>

/** @file
 * The SH-2's free-running timer: a 16-bit counter, two output compares and an input capture.
 *
 * Every register is a byte to the CPU. FRC, OCRA, OCRB and ICR are 16 bits each, as two bytes:
 * the high byte first, then the low one, never one alone. ICR is read alone.
 *
 * OCRA and OCRB share an address. TOCR's OCRS picks which of the two is there. */

#define SH2_TIER 0xFFFFFE10
#define SH2_FTCSR 0xFFFFFE11
#define SH2_FRCH 0xFFFFFE12
#define SH2_FRCL 0xFFFFFE13
#define SH2_OCRAH 0xFFFFFE14
#define SH2_OCRAL 0xFFFFFE15
#define SH2_OCRBH 0xFFFFFE14
#define SH2_OCRBL 0xFFFFFE15
#define SH2_TCR 0xFFFFFE16
#define SH2_TOCR 0xFFFFFE17
#define SH2_ICRH 0xFFFFFE18
#define SH2_ICRL 0xFFFFFE19

/* The interrupt enables. Bit 0 reads 1 and is written 1 */
#define SH2_TIER_ICIE 0x80
#define SH2_TIER_OCIAE 0x08
#define SH2_TIER_OCIBE 0x04
#define SH2_TIER_OVIE 0x02

/* The four flags are set by the timer. One is cleared by reading it as 1 and then writing it 0 */
#define SH2_FTCSR_ICF 0x80
#define SH2_FTCSR_OCFA 0x08
#define SH2_FTCSR_OCFB 0x04
#define SH2_FTCSR_OVF 0x02
#define SH2_FTCSR_CCLRA 0x01

/* The capture's edge, and the clock: CKS1 and CKS0 as 0 for the system clock by 8, 1 by 32,
 * 2 by 128, 3 for the external clock */
#define SH2_TCR_IEDGA 0x80
#define SH2_TCR_CKS1 0x02
#define SH2_TCR_CKS0 0x01

/* Bits 7 to 5 read 1 and are written 1 */
#define SH2_TOCR_OCRS 0x10
#define SH2_TOCR_OLVLA 0x02
#define SH2_TOCR_OLVLB 0x01

#ifndef __ASSEMBLER__

struct sh2_frt {
        uint8_t tier;
        uint8_t ftcsr;
        uint8_t frch;
        uint8_t frcl;
        /* OCRA or OCRB */
        uint8_t ocrh;
        uint8_t ocrl;
        uint8_t tcr;
        uint8_t tocr;
        uint8_t icrh;
        uint8_t icrl;
};

#define SH2_FRT ((volatile struct sh2_frt *)SH2_TIER)

HX_MAP_AT(struct sh2_frt, tier, SH2_TIER, SH2_TIER);
HX_MAP_AT(struct sh2_frt, ftcsr, SH2_TIER, SH2_FTCSR);
HX_MAP_AT(struct sh2_frt, frch, SH2_TIER, SH2_FRCH);
HX_MAP_AT(struct sh2_frt, frcl, SH2_TIER, SH2_FRCL);
HX_MAP_AT(struct sh2_frt, ocrh, SH2_TIER, SH2_OCRAH);
HX_MAP_AT(struct sh2_frt, ocrl, SH2_TIER, SH2_OCRAL);
HX_MAP_AT(struct sh2_frt, ocrh, SH2_TIER, SH2_OCRBH);
HX_MAP_AT(struct sh2_frt, ocrl, SH2_TIER, SH2_OCRBL);
HX_MAP_AT(struct sh2_frt, tcr, SH2_TIER, SH2_TCR);
HX_MAP_AT(struct sh2_frt, tocr, SH2_TIER, SH2_TOCR);
HX_MAP_AT(struct sh2_frt, icrh, SH2_TIER, SH2_ICRH);
HX_MAP_AT(struct sh2_frt, icrl, SH2_TIER, SH2_ICRL);

#endif /* !__ASSEMBLER__ */

#endif /* !HX_HW_SH2_FRT_H */
