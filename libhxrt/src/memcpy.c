#include <string.h>

#undef memcpy

/* A byte at a time, from the lowest. A block copied onto itself is written with its own
 * bytes. */
void *
memcpy(void *to, const void *from, size_t length)
{
        unsigned char *out = to;
        const unsigned char *in = from;

        for (size_t at = 0; at < length; at++) {
                out[at] = in[at];
        }
        return to;
}
