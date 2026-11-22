#include <string.h>

#undef memset

/* A byte at a time, from the lowest */
void *
memset(void *block, int value, size_t length)
{
        unsigned char *out = block;
        const unsigned char byte = (unsigned char)value;

        for (size_t at = 0; at < length; at++) {
                out[at] = byte;
        }
        return block;
}
