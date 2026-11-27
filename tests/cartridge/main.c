#include <stdint.h>

#include <hx/cart.h>
#include <hx/hw/sh2/cache.h>
#include <hx/test.h>
#include <hx/vdp2.h>

/* What startup found in the cartridge slot, against what the run puts there.
 *
 * EXPECT_ID is the ID byte of the cartridge the run is given. With a RAM cartridge's ID its
 * memory is expected, and with another nothing. JOINED says the build asks for the 1 Mbyte
 * cartridge as one run. */

#ifndef EXPECT_ID
#error EXPECT_ID is not defined
#endif

struct run {
        uint32_t start;
        uint32_t size;
};

#if EXPECT_ID == HX_CART_ID_1MB && defined(JOINED)
#define EXPECT_SIZE HX_CART_1MB
#define EXPECT_RUN_COUNT 1
static const struct run expect_runs[2] = { { 0x02580000, 0x100000 }, { 0, 0 } };
#elif EXPECT_ID == HX_CART_ID_1MB
#define EXPECT_SIZE HX_CART_1MB
#define EXPECT_RUN_COUNT 2
static const struct run expect_runs[2] = { { 0x02400000, 0x80000 }, { 0x02600000, 0x80000 } };
#elif EXPECT_ID == HX_CART_ID_4MB
#define EXPECT_SIZE HX_CART_4MB
#define EXPECT_RUN_COUNT 1
static const struct run expect_runs[2] = { { 0x02400000, 0x400000 }, { 0, 0 } };
#else
#define EXPECT_SIZE 0
#define EXPECT_RUN_COUNT 0
static const struct run expect_runs[2] = { { 0, 0 }, { 0, 0 } };
#endif

/* What each run's longs are written with, a value for each long */
static const uint32_t seeds[2] = { 0xC0DE0000, 0x1BAD0000 };

static uint32_t calls;
static uint32_t context_of_the_call;

/* Writes a run at its first long, at the long of each power of two, and at its last */
static void
write_run(const hx_cart_run_t *run, uint32_t seed)
{
        volatile uint32_t * const longs = (volatile uint32_t *)run->start;
        const uint32_t last = (run->size / 4) - 1;

        longs[0] = seed;
        for (uint32_t at = 1; at < last; at <<= 1) {
                longs[at] = seed + at;
        }
        longs[last] = seed + last;
}

/* The longs of a run that do not hold what write_run left, read at the run's address with
 * through added: through the cache, or past it */
static uint32_t
unlike(const hx_cart_run_t *run, uint32_t seed, uintptr_t through)
{
        const volatile uint32_t * const longs =
            (const volatile uint32_t *)((uintptr_t)run->start | through);
        const uint32_t last = (run->size / 4) - 1;
        uint32_t count = 0;

        if (longs[0] != seed) {
                count++;
        }
        for (uint32_t at = 1; at < last; at <<= 1) {
                if (longs[at] != seed + at) {
                        count++;
                }
        }
        if (longs[last] != seed + last) {
                count++;
        }
        return count;
}

static void
too_small(const hx_cart_t *found, void *context)
{
        calls++;
        hx_test_equal("require.less.calls", calls, 1);
        hx_test_equal("require.less.found", found == hx_cart_found(), 1);
        hx_test_equal("require.less.context", context == &context_of_the_call, 1);
        /* The picture ahead of the mark, so the mark is no sooner there than it has to be */
        hx_test_show();
        hx_test_end();
}

int
main(void)
{
        const hx_cart_t * const found = hx_cart_found();
        static const char * const past_the_cache[2] = { "holds.0", "holds.1" };
        static const char * const through_the_cache[2] = { "holds.0.cached", "holds.1.cached" };

        hx_test_equal("found.same", found == hx_cart_found(), 1);
        hx_test_equal("found.id", found->id, EXPECT_ID);
        hx_test_equal("found.size", found->size, EXPECT_SIZE);
        hx_test_equal("found.run-count", found->run_count, EXPECT_RUN_COUNT);
        hx_test_equal("found.0.start", (uint32_t)(uintptr_t)found->runs[0].start,
            expect_runs[0].start);
        hx_test_equal("found.0.size", found->runs[0].size, expect_runs[0].size);
        hx_test_equal("found.1.start", (uint32_t)(uintptr_t)found->runs[1].start,
            expect_runs[1].start);
        hx_test_equal("found.1.size", found->runs[1].size, expect_runs[1].size);

        /* Every run written ahead of any read, so a long that is two runs' shows */
        if (found->run_count <= 2) {
                for (uint32_t run = 0; run < found->run_count; run++) {
                        write_run(&found->runs[run], seeds[run]);
                }
                for (uint32_t run = 0; run < found->run_count; run++) {
                        hx_test_equal(past_the_cache[run],
                            unlike(&found->runs[run], seeds[run], SH2_CACHE_THROUGH), 0);
                        hx_test_equal(through_the_cache[run],
                            unlike(&found->runs[run], seeds[run], 0), 0);
                }
        }

        /* A size that is there: the call returns, and the function is not called */
        hx_cart_require(0, too_small, &context_of_the_call);
        hx_test_equal("require.none.calls", calls, 0);
        hx_cart_require(found->size, too_small, &context_of_the_call);
        hx_test_equal("require.all.calls", calls, 0);

        /* A byte more: the function is called, and ends the report. It returns, and the call
         * is not to */
        hx_cart_require(found->size + 1, too_small, &context_of_the_call);

        /* Reached only when the call returned. The report loses its mark within the frame, and
         * the run looks for it after each frame, so it finds none. The screen loses its text */
        hx_test_report[0] = 0;
        vdp2_start();
        for (;;) {
        }
}
