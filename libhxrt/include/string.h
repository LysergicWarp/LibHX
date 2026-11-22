#ifndef HX_STRING_H
#define HX_STRING_H

#include <stddef.h>

/** @file
 * The C library's <string.h>. Its four memory functions are all it holds so far.
 *
 * The compiler calls memcpy and memset on its own, for an object copied or zeroed, so a
 * program may link them without naming them.
 *
 * Past what the C standard says:
 *   - A length of 0 reads and writes nothing, whatever the pointers, a null one too.
 *   - A null pointer with a length is not checked. Address 0 is the BIOS ROM: a read gives
 *     its bytes and a write is lost.
 *   - The width and the order of the accesses are not fixed. The functions are for memory,
 *     not for hardware registers.
 *
 * The functions keep no state. They may be called anywhere, in an interrupt handler and on
 * the second SH-2 too.
 *
 * Each name is a macro on the compiler's builtin as well, so a call with a small fixed length
 * may be expanded in place. (memcpy)(to, from, length) calls the function itself. */

#ifdef __cplusplus
extern "C" {
#endif

/** Copies length bytes from from to to, and returns to. The two blocks do not overlap, but
 * for a block copied onto itself, which is left as it was */
void *memcpy(void *to, const void *from, size_t length);

/** Copies length bytes from from to to, and returns to. The two blocks may overlap */
void *memmove(void *to, const void *from, size_t length);

/** Sets length bytes of block to value, taken as an unsigned char, and returns block */
void *memset(void *block, int value, size_t length);

/** Compares length bytes of the two blocks, each byte as an unsigned char. The result is 0
 * when they are the same. Otherwise it is negative when the first byte that differs is the
 * smaller in left, and positive when it is the greater. Only its sign is fixed */
int memcmp(const void *left, const void *right, size_t length);

#ifdef __cplusplus
}
#endif

/** @cond */
/* Any arguments, so one with a comma between braces is taken whole */
#define memcpy(...) __builtin_memcpy(__VA_ARGS__)
#define memmove(...) __builtin_memmove(__VA_ARGS__)
#define memset(...) __builtin_memset(__VA_ARGS__)
#define memcmp(...) __builtin_memcmp(__VA_ARGS__)
/** @endcond */

#endif /* !HX_STRING_H */
