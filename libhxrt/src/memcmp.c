#include <string.h>

#undef memcmp

/* A byte at a time, up to the first that differs */
int
memcmp(const void *left, const void *right, size_t length)
{
        const unsigned char *one = left;
        const unsigned char *other = right;

        for (size_t at = 0; at < length; at++) {
                if (one[at] != other[at]) {
                        return (int)one[at] - (int)other[at];
                }
        }
        return 0;
}
