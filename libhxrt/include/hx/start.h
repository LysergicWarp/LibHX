#ifndef HX_START_H
#define HX_START_H

/** @file
 * Startup, and where a program's data goes.
 *
 * The BIOS loads the program to Work RAM-H and enters it at startup. Startup sets the stack,
 * turns the cache on, clears the zeroed data, copies the initial values that belong outside
 * Work RAM-H, looks into the cartridge slot, and calls main without arguments. A return from
 * main stops in a loop.
 *
 * main finds:
 *   - the stack pointer at the stack's end
 *   - the cache purged and on, with four ways, for instructions and data both
 *   - every object without an initial value zeroed, and every other holding its own
 *   - a RAM cartridge set up, when one is in the slot, and what was found there kept
 *     (<hx/cart.h>)
 *   - the vector table, GBR and the interrupt masks of the SH-2 and the SCU as the BIOS left
 *     them. The table is the BIOS's own, and the SH-2's mask is open.
 *
 * Startup is entered once, by the BIOS. The initial values in Work RAM-H are the loaded file's
 * own bytes, so a second entry would find them as the program left them.
 *
 * A constructor or a destructor of a static object is not run, and a program with one does
 * not link.
 *
 * The stack is at the top of Work RAM-H, 16 kbytes, or below the program: over the Initial
 * Program and down to 0x06001000, which the second SH-2's stack grows down from when the
 * disc's header names none. The program's build picks the place and the size (HX_STACK and
 * HX_STACK_SIZE, build/program.mk). */

/** An object placed in Work RAM-L instead of Work RAM-H:
 *
 *      HX_LOW_RAM static uint8_t buffer[0x40000];
 *      HX_LOW_RAM_DATA static uint16_t steps[] = { 1, 2, 4 };
 *
 * HX_LOW_RAM is for one without an initial value: zeroed, and not in the file the BIOS loads.
 * HX_LOW_RAM_DATA is for one with: its value is in the file, and startup copies it over. */
#define HX_LOW_RAM HX_PRIV_SECTION(".bss.hx_low_ram", __LINE__)
/** @copydoc HX_LOW_RAM */
#define HX_LOW_RAM_DATA HX_PRIV_SECTION(".data.hx_low_ram", __LINE__)

/* A section for each line that places an object, so a link drops the ones nothing uses */
#define HX_PRIV_SECTION(name, line) HX_PRIV_SECTION_NAMED(name, line)
#define HX_PRIV_SECTION_NAMED(name, line) __attribute__((section(name "." #line)))

#ifdef __cplusplus
extern "C" {
#endif

/** The stack: its lowest byte, and the byte past its highest */
extern char hx_stack_start[];
/** @copydoc hx_stack_start */
extern char hx_stack_end[];

#ifdef __cplusplus
}
#endif

#endif /* !HX_START_H */
