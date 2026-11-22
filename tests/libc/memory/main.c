#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <hx/test.h>

/* memcpy, memmove, memset and memcmp: every length up to LONGEST, with each block at every
 * alignment in a long.
 *
 * The sweeps call the functions by their names in brackets, which reach the library's own
 * and not the compiler's expansion of the header's macros. The last rows go through the
 * macros, and through the calls the compiler makes on its own.
 *
 * A sweep gives three rows: its cases, the ones that failed, and the first of those, as its
 * length and where its blocks were. */

#define LONGEST 72U

/* How far apart the two blocks of a move within one block start, at most */
#define FARTHEST 17U

/* The bytes at each end of a block that no case is to write */
#define EDGE 24U

#define START (EDGE + FARTHEST)
#define BLOCK (START + 4U + LONGEST + FARTHEST + EDGE)

#define NONE 0xFFFFFFFFU

struct sweep {
        uint32_t cases;
        uint32_t failed;
        uint32_t first;
};

struct odd {
        char bytes[7];
};

struct large {
        uint32_t longs[64];
};

static alignas(4) unsigned char from_block[BLOCK];
static alignas(4) unsigned char to_block[BLOCK];
static alignas(4) unsigned char wanted_block[BLOCK];

static const int values[] = { 0, 0xFF, 0x5A, 0x1A5, -1 };

static struct odd odds[3];
static struct large larges[3];

/* Null, and not known to the compiler as such */
static void * volatile nothing;

/* A block's bytes ahead of a case. No two of a block are the same, and none is the same as
 * the other block's at its place. */
static unsigned char
from_byte(size_t at)
{
        return (unsigned char)((at * 3U) + 1U);
}

static unsigned char
to_byte(size_t at)
{
        return (unsigned char)((at * 5U) + 2U);
}

static void
fill_from(unsigned char *block)
{
        for (size_t at = 0; at < BLOCK; at++) {
                block[at] = from_byte(at);
        }
}

static void
fill_to(unsigned char *block)
{
        for (size_t at = 0; at < BLOCK; at++) {
                block[at] = to_byte(at);
        }
}

static bool
is_from(const unsigned char *block)
{
        for (size_t at = 0; at < BLOCK; at++) {
                if (block[at] != from_byte(at)) {
                        return false;
                }
        }
        return true;
}

static bool
same(const unsigned char *block, const unsigned char *wanted)
{
        for (size_t at = 0; at < BLOCK; at++) {
                if (block[at] != wanted[at]) {
                        return false;
                }
        }
        return true;
}

static uint32_t
case_of(size_t length, size_t middle, size_t low)
{
        return (uint32_t)((length << 16) | (middle << 8) | low);
}

static void
note(struct sweep *sweep, bool pass, uint32_t number)
{
        sweep->cases++;
        if (!pass) {
                if (sweep->failed == 0) {
                        sweep->first = number;
                }
                sweep->failed++;
        }
}

static void
rows(const char *cases, const char *failed, const char *first, const struct sweep *sweep,
    uint32_t wanted)
{
        hx_test_equal(cases, sweep->cases, wanted);
        hx_test_equal(failed, sweep->failed, 0);
        hx_test_equal(first, sweep->first, NONE);
}

/* Between two blocks: the bytes copied, every other byte of both as it was, and to returned.
 * A case is its length, then where to and from were in a long. */
static struct sweep
sweep_apart(void *(*copy)(void *, const void *, size_t))
{
        struct sweep sweep = { 0, 0, NONE };

        for (size_t length = 0; length <= LONGEST; length++) {
                for (size_t to_at = 0; to_at < 4; to_at++) {
                        for (size_t from_at = 0; from_at < 4; from_at++) {
                                unsigned char * const to = to_block + START + to_at;
                                const unsigned char * const from =
                                    from_block + START + from_at;
                                const void *returned;

                                fill_from(from_block);
                                fill_to(to_block);
                                fill_to(wanted_block);
                                for (size_t at = 0; at < length; at++) {
                                        wanted_block[START + to_at + at] = from[at];
                                }
                                returned = copy(to, from, length);
                                note(&sweep,
                                    returned == to && same(to_block, wanted_block) &&
                                    is_from(from_block),
                                    case_of(length, to_at, from_at));
                        }
                }
        }
        return sweep;
}

/* A block copied onto itself by memcpy: left as it was. A case is its length, then where the
 * block was in a long. */
static struct sweep
sweep_itself(void)
{
        struct sweep sweep = { 0, 0, NONE };

        for (size_t length = 0; length <= LONGEST; length++) {
                for (size_t block_at = 0; block_at < 4; block_at++) {
                        unsigned char * const block = to_block + START + block_at;
                        const void *returned;

                        fill_from(to_block);
                        returned = (memcpy)(block, block, length);
                        note(&sweep, returned == block && is_from(to_block),
                            case_of(length, 0, block_at));
                }
        }
        return sweep;
}

/* Within one block by memmove, the target from FARTHEST below the source to FARTHEST above.
 * A case is its length, then the target's start counted from FARTHEST below the source's,
 * then where the source was in a long. */
static struct sweep
sweep_within(void)
{
        struct sweep sweep = { 0, 0, NONE };

        for (size_t length = 0; length <= LONGEST; length++) {
                for (size_t block_at = 0; block_at < 4; block_at++) {
                        for (size_t shift = 0; shift <= 2U * FARTHEST; shift++) {
                                const size_t from_at = START + block_at;
                                const size_t to_at = from_at - FARTHEST + shift;
                                const void *returned;

                                fill_from(to_block);
                                fill_from(wanted_block);
                                for (size_t at = 0; at < length; at++) {
                                        wanted_block[to_at + at] = from_byte(from_at + at);
                                }
                                returned = (memmove)(to_block + to_at, to_block + from_at,
                                    length);
                                note(&sweep,
                                    returned == to_block + to_at &&
                                    same(to_block, wanted_block),
                                    case_of(length, shift, block_at));
                        }
                }
        }
        return sweep;
}

/* The bytes set, every other byte as it was, and the block returned. A case is its length,
 * then which of the values, then where the block was in a long. */
static struct sweep
sweep_set(void)
{
        struct sweep sweep = { 0, 0, NONE };

        for (size_t length = 0; length <= LONGEST; length++) {
                for (size_t value = 0; value < sizeof values / sizeof values[0]; value++) {
                        for (size_t block_at = 0; block_at < 4; block_at++) {
                                unsigned char * const block = to_block + START + block_at;
                                const void *returned;

                                fill_from(to_block);
                                fill_from(wanted_block);
                                for (size_t at = 0; at < length; at++) {
                                        wanted_block[START + block_at + at] =
                                            (unsigned char)values[value];
                                }
                                returned = (memset)(block, values[value], length);
                                note(&sweep,
                                    returned == block && same(to_block, wanted_block),
                                    case_of(length, value, block_at));
                        }
                }
        }
        return sweep;
}

/* Two blocks with the same length bytes, and other bytes on either side of them */
static void
make_same(size_t length, size_t left_at, size_t right_at)
{
        const size_t left = START + left_at;
        const size_t right = START + right_at;

        fill_from(from_block);
        fill_to(to_block);
        for (size_t at = 0; at < length; at++) {
                to_block[right + at] = from_block[left + at];
        }
        to_block[right - 1] = (unsigned char)~from_block[left - 1];
        to_block[right + length] = (unsigned char)~from_block[left + length];
}

/* The same bytes compare as 0, whatever lies on either side. A case is its length, then
 * where left and right were in a long. */
static struct sweep
sweep_same(void)
{
        struct sweep sweep = { 0, 0, NONE };

        for (size_t length = 0; length <= LONGEST; length++) {
                for (size_t left_at = 0; left_at < 4; left_at++) {
                        for (size_t right_at = 0; right_at < 4; right_at++) {
                                make_same(length, left_at, right_at);
                                note(&sweep,
                                    (memcmp)(from_block + START + left_at,
                                    to_block + START + right_at, length) == 0,
                                    case_of(length, left_at, right_at));
                        }
                }
        }
        return sweep;
}

/* One byte the smaller in left, as unsigned chars, at each place in turn, and the byte after
 * it the greater: the first that differs gives the sign, both ways round. A case is its
 * length, then the place, then where left and right were in a long. */
static struct sweep
sweep_differ(void)
{
        struct sweep sweep = { 0, 0, NONE };

        for (size_t length = 1; length <= LONGEST; length++) {
                for (size_t left_at = 0; left_at < 4; left_at++) {
                        for (size_t right_at = 0; right_at < 4; right_at++) {
                                unsigned char * const left = from_block + START + left_at;
                                unsigned char * const right = to_block + START + right_at;

                                make_same(length, left_at, right_at);
                                for (size_t place = 0; place < length; place++) {
                                        const bool next = place + 1 < length;
                                        const unsigned char kept = left[place];
                                        const unsigned char kept_next = left[place + 1];

                                        left[place] = 0x7F;
                                        right[place] = 0x80;
                                        if (next) {
                                                left[place + 1] = 0xFF;
                                                right[place + 1] = 0x00;
                                        }
                                        note(&sweep,
                                            (memcmp)(left, right, length) < 0 &&
                                            (memcmp)(right, left, length) > 0,
                                            case_of(length, place, (left_at << 2) | right_at));
                                        left[place] = kept;
                                        right[place] = kept;
                                        if (next) {
                                                left[place + 1] = kept_next;
                                                right[place + 1] = kept_next;
                                        }
                                }
                        }
                }
        }
        return sweep;
}

/* The compiler makes this a call of memcpy, and the next a call of memset */
__attribute__((noipa)) static void
copy_odd(struct odd *to, const struct odd *from)
{
        *to = *from;
}

__attribute__((noipa)) static void
clear_large(struct large *large)
{
        *large = (struct large){ 0 };
}

/* The bytes of the three that are not as a copy into the middle one leaves them */
static uint32_t
odd_copied(void)
{
        struct odd source;
        uint32_t wrong = 0;

        for (size_t at = 0; at < sizeof source.bytes; at++) {
                source.bytes[at] = (char)(at + 1);
                odds[0].bytes[at] = 0x11;
                odds[1].bytes[at] = 0x22;
                odds[2].bytes[at] = 0x33;
        }
        copy_odd(&odds[1], &source);
        for (size_t at = 0; at < sizeof source.bytes; at++) {
                wrong += odds[0].bytes[at] != 0x11;
                wrong += odds[1].bytes[at] != (char)(at + 1);
                wrong += odds[2].bytes[at] != 0x33;
        }
        return wrong;
}

/* The longs of the three that are not as a clear of the middle one leaves them */
static uint32_t
large_cleared(void)
{
        const size_t count = sizeof larges[0].longs / sizeof larges[0].longs[0];
        uint32_t wrong = 0;

        for (size_t at = 0; at < count; at++) {
                larges[0].longs[at] = NONE;
                larges[1].longs[at] = NONE;
                larges[2].longs[at] = NONE;
        }
        clear_large(&larges[1]);
        for (size_t at = 0; at < count; at++) {
                wrong += larges[0].longs[at] != NONE;
                wrong += larges[1].longs[at] != 0;
                wrong += larges[2].longs[at] != NONE;
        }
        return wrong;
}

int
main(void)
{
        struct sweep sweep;

        sweep = sweep_apart(memcpy);
        rows("memcpy.cases", "memcpy.failed", "memcpy.first", &sweep, (LONGEST + 1U) * 16U);
        sweep = sweep_itself();
        rows("memcpy.itself.cases", "memcpy.itself.failed", "memcpy.itself.first", &sweep,
            (LONGEST + 1U) * 4U);
        hx_test_equal("memcpy.null", (uintptr_t)(memcpy)(nothing, nothing, 0), 0);

        sweep = sweep_apart(memmove);
        rows("memmove.apart.cases", "memmove.apart.failed", "memmove.apart.first", &sweep,
            (LONGEST + 1U) * 16U);
        sweep = sweep_within();
        rows("memmove.within.cases", "memmove.within.failed", "memmove.within.first", &sweep,
            (LONGEST + 1U) * 4U * ((2U * FARTHEST) + 1U));
        hx_test_equal("memmove.null", (uintptr_t)(memmove)(nothing, nothing, 0), 0);

        sweep = sweep_set();
        rows("memset.cases", "memset.failed", "memset.first", &sweep,
            (LONGEST + 1U) * 4U * (sizeof values / sizeof values[0]));
        hx_test_equal("memset.null", (uintptr_t)(memset)(nothing, 0x5A, 0), 0);

        sweep = sweep_same();
        rows("memcmp.same.cases", "memcmp.same.failed", "memcmp.same.first", &sweep,
            (LONGEST + 1U) * 16U);
        sweep = sweep_differ();
        rows("memcmp.differ.cases", "memcmp.differ.failed", "memcmp.differ.first", &sweep,
            (LONGEST * (LONGEST + 1U) / 2U) * 16U);
        hx_test_equal("memcmp.null", (uint32_t)(memcmp)(nothing, nothing, 0), 0);

        hx_test_equal("compiler.memcpy", odd_copied(), 0);
        hx_test_equal("compiler.memset", large_cleared(), 0);

        /* By name, through the header's macros */
        fill_from(from_block);
        fill_to(to_block);
        fill_to(wanted_block);
        memcpy(to_block + 5, from_block + 9, 4);
        for (size_t at = 0; at < 4; at++) {
                wanted_block[5 + at] = from_byte(9 + at);
        }
        hx_test_equal("macro.memcpy", same(to_block, wanted_block), 1);
        memset(to_block + 8, 0x5A, 8);
        for (size_t at = 0; at < 8; at++) {
                wanted_block[8 + at] = 0x5A;
        }
        hx_test_equal("macro.memset", same(to_block, wanted_block), 1);
        memmove(to_block + 33, to_block + 32, 8);
        for (size_t at = 0; at < 8; at++) {
                wanted_block[33 + at] = to_byte(32 + at);
        }
        hx_test_equal("macro.memmove", same(to_block, wanted_block), 1);
        /* An argument with commas of its own */
        memcpy(to_block + 48, &(struct odd){ { 1, 2, 3, 4, 5, 6, 7 } }, sizeof (struct odd));
        for (size_t at = 0; at < sizeof (struct odd); at++) {
                wanted_block[48 + at] = (unsigned char)(at + 1);
        }
        hx_test_equal("macro.commas", same(to_block, wanted_block), 1);
        hx_test_equal("macro.memcmp", (uint32_t)memcmp(to_block, wanted_block, BLOCK), 0);
        to_block[40] = 0x7F;
        wanted_block[40] = 0x80;
        hx_test_equal("macro.memcmp.differs", memcmp(to_block, wanted_block, BLOCK) < 0, 1);

        hx_test_end();
        hx_test_show();
        return 0;
}
