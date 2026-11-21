#ifndef HX_HW_SH2_CACHE_H
#define HX_HW_SH2_CACHE_H

#include <hx/hw/map.h>

/** @file
 * The SH-2's cache: 4 kbytes, four ways of 64 lines, 16 bytes a line, instructions and data
 * together.
 *
 * A reset leaves the cache off and its lines as they were. It is purged ahead of being turned
 * on, and CCR is written with the cache off or going off. */

/* Read and written as a byte */
#define SH2_CCR 0xFFFFFE92

/* W1 and W0 are the way that the address array shows. CP purges every line and reads 0. TW
 * leaves ways 2 and 3 as the cache and ways 0 and 1 as RAM. OD and ID keep a data read and an
 * instruction fetch that miss from filling a line. CE turns the cache on */
#define SH2_CCR_W1 0x80
#define SH2_CCR_W0 0x40
#define SH2_CCR_CP 0x10
#define SH2_CCR_TW 0x08
#define SH2_CCR_OD 0x04
#define SH2_CCR_ID 0x02
#define SH2_CCR_CE 0x01

/* An address's top three bits pick how the cache takes the access. Memory is reached through the
 * cache from 0 and past it from SH2_CACHE_THROUGH: the same memory at both.
 *
 * A long written at an address plus SH2_CACHE_PURGE drops the line that holds the address, from
 * every way.
 *
 * The address array is read and written as longs, the way by W1 and W0. The data array is the
 * lines themselves, a way after another, 1 kbyte each */
#define SH2_CACHE_THROUGH 0x20000000
#define SH2_CACHE_PURGE 0x40000000
#define SH2_CACHE_ADDRESS_ARRAY 0x60000000
#define SH2_CACHE_DATA_ARRAY 0xC0000000

#define SH2_CACHE_LINE_SIZE 16

#ifndef __ASSEMBLER__

struct sh2_cache {
        uint8_t ccr;
};

#define SH2_CACHE ((volatile struct sh2_cache *)SH2_CCR)

HX_MAP_AT(struct sh2_cache, ccr, SH2_CCR, SH2_CCR);

#endif /* !__ASSEMBLER__ */

#endif /* !HX_HW_SH2_CACHE_H */
