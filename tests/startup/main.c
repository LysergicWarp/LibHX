#include <stdint.h>

#include <hx/cart.h>
#include <hx/hw/sh2/cache.h>
#include <hx/start.h>
#include <hx/test.h>

/* What main finds after startup.
 *
 * Startup runs twice. The BIOS enters it on memory that may be zero already and with a cache
 * that may be on, so the first main writes over all that startup is to set, turns the cache
 * off and enters startup again. The second main holds the rows. The first leaves Work RAM-H's
 * initial values alone, which no second entry would bring back. STACK_LOW says the stack is
 * below the program. */

/* Startup, the program's first byte */
void hx_start(void);

/* The linker script's own names for what startup clears and copies */
extern volatile uint32_t bss_start[] __asm__("hx_bss");
extern volatile uint32_t bss_end[] __asm__("hx_bss_end");
extern volatile uint32_t low_bss_start[] __asm__("hx_low_ram_bss");
extern volatile uint32_t low_bss_end[] __asm__("hx_low_ram_bss_end");
extern volatile uint32_t low_data_start[] __asm__("hx_low_ram_data");
extern volatile uint32_t low_data_end[] __asm__("hx_low_ram_data_end");
extern volatile uint32_t low_data_load[] __asm__("hx_low_ram_data_load");

/* Says the first main has run. In the stack's lowest long, which no call here reaches */
#define SECOND 0x326E6421U

#define WORK_RAM_LOW 0x002U
#define HIGH_RAM_END 0x06100000U
#define LOW_STACK_START 0x06001000U
#define STACK_SIZE 0x4000U

/* main's own frame and what startup may have pushed are within this of the stack's end */
#define STACK_USED_AT_MOST 0x100U

static volatile uint32_t data = 0x11223344;
static volatile uint16_t data_words[3] = { 0x5566, 0x7788, 0x99AA };
static volatile uint32_t bss;
static volatile uint8_t bss_bytes[5];

HX_LOW_RAM_DATA static volatile uint32_t low_data = 0xBBCCDDEE;
HX_LOW_RAM_DATA static volatile uint8_t low_data_bytes[3] = { 0x12, 0x34, 0x56 };
HX_LOW_RAM static volatile uint32_t low_bss;
HX_LOW_RAM static volatile uint8_t low_bss_bytes[7];

static volatile int32_t dividend = -1000000;
static volatile int32_t divisor = 7;
static volatile uint32_t unsigned_dividend = 0xFFFFFFFF;
static volatile uint32_t unsigned_divisor = 10;

static uint32_t
megabyte(const volatile void *object)
{
        return (uint32_t)(uintptr_t)object >> 20;
}

static void
fill(volatile uint32_t *start, const volatile uint32_t *end)
{
        while (start < end) {
                *start = 0xFFFFFFFF;
                start++;
        }
}

/* Every bit that is set from start up to end */
static uint32_t
left(const volatile uint32_t *start, const volatile uint32_t *end)
{
        uint32_t bits = 0;

        while (start < end) {
                bits |= *start;
                start++;
        }
        return bits;
}

/* The longs of Work RAM-L's data that are not the loaded file's */
static uint32_t
unlike(void)
{
        const volatile uint32_t *loaded = low_data_load;
        uint32_t count = 0;

        for (const volatile uint32_t *at = low_data_start; at < low_data_end; at++) {
                if (*at != *loaded) {
                        count++;
                }
                loaded++;
        }
        return count;
}

int
main(void)
{
        volatile uint32_t * const second = (volatile uint32_t *)hx_stack_start;
        const uintptr_t stack_start = (uintptr_t)hx_stack_start;
        const uintptr_t stack_end = (uintptr_t)hx_stack_end;
        const uintptr_t frame = (uintptr_t)__builtin_frame_address(0);
        /* What startup found in the cartridge slot is in .bss, and filled after the clear */
        const volatile uint32_t * const cart = (const volatile uint32_t *)hx_cart_found();
        const volatile uint32_t * const cart_end = cart + (sizeof(hx_cart_t) / sizeof *cart);
        uint32_t bss_left;
        uint32_t low_bss_left;
        uint32_t bytes;

        if (*second != SECOND) {
                /* The report and its state are in .bss too. Nothing here needs them */
                fill(bss_start, bss_end);
                fill(low_bss_start, low_bss_end);
                fill(low_data_start, low_data_end);
                SH2_CACHE->ccr = 0;
                *second = SECOND;
                hx_start();
        }
        *second = 0;

        /* Read ahead of the first row, which writes to .bss */
        bss_left = left(bss_start, cart) | left(cart_end, bss_end);
        low_bss_left = left(low_bss_start, low_bss_end);

        hx_test_equal("data", data, 0x11223344);
        hx_test_equal("data.words",
            ((uint32_t)data_words[0] << 16) | data_words[1], 0x55667788);
        hx_test_equal("data.words.last", data_words[2], 0x99AA);
        hx_test_equal("bss.whole", bss_left, 0);
        /* The record's two bytes that startup never fills */
        hx_test_equal("bss.cart", cart[1] & 0xFFFF, 0);
        hx_test_equal("bss.size",
            (uintptr_t)bss_end - (uintptr_t)bss_start >= HX_TEST_REPORT_SIZE, 1);
        hx_test_equal("bss", bss, 0);
        bytes = 0;
        for (uint32_t at = 0; at < sizeof bss_bytes; at++) {
                bytes |= bss_bytes[at];
        }
        hx_test_equal("bss.bytes", bytes, 0);

        hx_test_equal("low-ram.data.whole", unlike(), 0);
        hx_test_equal("low-ram.data.size",
            (uintptr_t)low_data_end - (uintptr_t)low_data_start,
            8);
        hx_test_equal("low-ram.data", low_data, 0xBBCCDDEE);
        hx_test_equal("low-ram.data.bytes",
            ((uint32_t)low_data_bytes[0] << 16) | ((uint32_t)low_data_bytes[1] << 8) |
            low_data_bytes[2], 0x123456);
        hx_test_equal("low-ram.data.place", megabyte(&low_data), WORK_RAM_LOW);
        hx_test_equal("low-ram.data.bytes.place", megabyte(low_data_bytes), WORK_RAM_LOW);
        hx_test_equal("low-ram.bss.whole", low_bss_left, 0);
        hx_test_equal("low-ram.bss.size",
            (uintptr_t)low_bss_end - (uintptr_t)low_bss_start, 12);
        hx_test_equal("low-ram.bss", low_bss, 0);
        bytes = 0;
        for (uint32_t at = 0; at < sizeof low_bss_bytes; at++) {
                bytes |= low_bss_bytes[at];
        }
        hx_test_equal("low-ram.bss.bytes", bytes, 0);
        hx_test_equal("low-ram.bss.place", megabyte(&low_bss), WORK_RAM_LOW);
        hx_test_equal("low-ram.bss.bytes.place", megabyte(low_bss_bytes), WORK_RAM_LOW);

#ifdef STACK_LOW
        hx_test_equal("stack.start", stack_start, LOW_STACK_START);
        hx_test_equal("stack.end", stack_end, (uintptr_t)hx_start);
#else
        hx_test_equal("stack.start", stack_start, HIGH_RAM_END - STACK_SIZE);
        hx_test_equal("stack.end", stack_end, HIGH_RAM_END);
#endif
        hx_test_equal("stack.pointer",
            frame <= stack_end && stack_end - frame <= STACK_USED_AT_MOST, 1);

        hx_test_equal("cache.ccr", SH2_CACHE->ccr, SH2_CCR_CE);

        hx_test_equal("divide", (uint32_t)(dividend / divisor), (uint32_t)-142857);
        hx_test_equal("divide.remainder", (uint32_t)(dividend % divisor), (uint32_t)-1);
        hx_test_equal("divide.unsigned", unsigned_dividend / unsigned_divisor, 429496729);
        hx_test_equal("divide.unsigned.remainder", unsigned_dividend % unsigned_divisor, 5);

        hx_test_end();
        hx_test_show();
        return 0;
}
