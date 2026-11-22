#include <stdint.h>
#include <string.h>

#undef memmove

/* A byte at a time: from the lowest when the target is the lower block, from the highest when
 * it is the higher, so a byte is read before it is written over. The addresses are compared
 * as numbers, since the two blocks may be of different objects. */
void *
memmove(void *to, const void *from, size_t length)
{
        unsigned char *out = to;
        const unsigned char *in = from;

        if ((uintptr_t)to <= (uintptr_t)from) {
                for (size_t at = 0; at < length; at++) {
                        out[at] = in[at];
                }
        } else {
                for (size_t at = length; at > 0; at--) {
                        out[at - 1] = in[at - 1];
                }
        }
        return to;
}
