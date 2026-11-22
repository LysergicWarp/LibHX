#ifndef HX_CART_H
#define HX_CART_H

/** @file
 * The cartridge slot: what startup found there, and the Extended RAM Cartridge's memory.
 *
 * Startup reads the slot's ID byte. For the ID of a RAM cartridge, 1 Mbyte or 4 Mbytes, it sets
 * the cartridge up and keeps where its memory is. For every other ID it writes nothing to the
 * slot. It never stops for want of a cartridge: a program asks what was found, or insists on a
 * size with hx_cart_require.
 *
 * The memory comes as runs, each an address and a size:
 *   - 4 Mbytes: one run.
 *   - 1 Mbyte: two runs of 512 kbytes with a gap between them, as Sega's manual for the
 *     cartridge gives them. No object there is larger than a run. A program built with
 *     HX_CART_JOIN as 1 (build/program.mk) may find one run of 1 Mbyte instead.
 *
 * A run's address is the one through the SH-2's cache. The memory is not cleared, and holds no
 * initial values. It is for data: no code runs from it. The SCU's DMA does not write to it.
 *
 * Startup's set-up is the only one. A change of the system clock loses the memory's contents,
 * and the cartridge is to be set up again after it.
 *
 * Nothing is linked into the cartridge, so one program runs with it and without. */

/** The memory of the two RAM cartridges, in bytes */
#define HX_CART_1MB 0x100000
/** @copydoc HX_CART_1MB */
#define HX_CART_4MB 0x400000

/** The ID bytes of the two RAM cartridges */
#define HX_CART_ID_1MB 0x5A
/** @copydoc HX_CART_ID_1MB */
#define HX_CART_ID_4MB 0x5C

#ifndef __ASSEMBLER__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A run of the cartridge's memory: its first byte, and its size in bytes */
typedef struct {
        void *start;
        uint32_t size;
} hx_cart_run_t;

/** What startup found in the cartridge slot.
 *
 * size is the memory of all runs together: HX_CART_1MB, HX_CART_4MB, or 0 with no RAM
 * cartridge there. id is the ID byte as it was read, whatever the cartridge. run_count is
 * how many of runs are filled, from the first. The others are 0. */
typedef struct {
        uint32_t size;
        uint8_t id;
        uint8_t run_count;
        hx_cart_run_t runs[2];
} hx_cart_t;

/** What startup found. The same record on every call, and startup alone writes it.
 *
 * It may be called anywhere, in an interrupt handler and on the second SH-2 too. */
const hx_cart_t *hx_cart_found(void);

/** A program's function for a cartridge that is missing or too small: found is what
 * hx_cart_found gives, and context is the one given to hx_cart_require */
typedef void hx_cart_short_t(const hx_cart_t *found, void *context);

/** Returns when the cartridge's memory is size bytes or more, so the code after the call can
 * count on it.
 *
 * With less memory it calls handler, once, for the program to say so on screen. If handler
 * returns, the call stops in a loop, as a return from main does. A size of 0 always returns.
 *
 * handler is a function. It is not checked for a null pointer. */
void hx_cart_require(uint32_t size, hx_cart_short_t *handler, void *context);

#ifdef __cplusplus
}
#endif

#endif /* !__ASSEMBLER__ */

#endif /* !HX_CART_H */
