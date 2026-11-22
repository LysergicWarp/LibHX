# The cartridge

Startup looks into the cartridge slot and sets an Extended RAM Cartridge up when one is there.
`<hx/cart.h>` says what it found. A program runs with a cartridge and without, and one that
needs the memory says so with a call.

## Ask what is there

```c
#include <stdint.h>

#include <hx/cart.h>

static uint8_t small_cache[0x8000];

int
main(void)
{
        const hx_cart_t * const cart = hx_cart_found();
        uint8_t *cache = small_cache;
        uint32_t cache_size = sizeof small_cache;

        if (cart->run_count > 0) {
                cache = cart->runs[0].start;
                cache_size = cart->runs[0].size;
        }
        cache[0] = 1;
        cache[cache_size - 1] = 2;

        for (;;) {
        }
}
```

The memory comes as runs, each an address and a size:

| In the slot | `size` | `run_count` | Runs |
| --- | --- | --- | --- |
| No cartridge, or another kind | 0 | 0 | None |
| The 1 Mbyte RAM cartridge | `HX_CART_1MB` | 2 | Two of 512 kbytes, with a gap between |
| The 4 Mbyte RAM cartridge | `HX_CART_4MB` | 1 | One of 4 Mbytes |

- `id` is the slot's ID byte as it was read, whatever the cartridge.
- A run's address is the one through the SH-2's cache.
- The memory is not cleared, and nothing is linked into it. The program fills it.
- No object in the 1 Mbyte cartridge is larger than a run.

## Insist on a size

```c
#include <stddef.h>
#include <stdint.h>

#include <hx/cart.h>

static void
no_cartridge(const hx_cart_t *found, void *context)
{
        (void)context;

        if (found->size == 0) {
                /* Say that the game needs the 4 Mbyte RAM cartridge */
        } else {
                /* Say that the cartridge in the slot is too small */
        }
}

int
main(void)
{
        uint8_t *textures;

        hx_cart_require(HX_CART_4MB, no_cartridge, NULL);

        textures = hx_cart_found()->runs[0].start;
        textures[0] = 1;

        for (;;) {
        }
}
```

- With 4 Mbytes there, the call returns and the code after it can count on the memory.
- With less, it calls the program's function, which is told what was found.
- If the function returns, the call stops in a loop.
- The size is in bytes. A program that asks for `HX_CART_1MB` passes with either cartridge.

> [!NOTE]
> hx draws no text yet. The function can show a colour, with
> [the back screen](vdp2.md#a-colour-on-screen).

## The disc's header

A disc that uses the RAM cartridge names it among its peripherals, with `W`:

```make
HX_IP_PERIPHERALS:= JW
```

## What the memory is for

- Data. No code runs from the cartridge.
- Sega's manual gives its access as about four times slower than Work RAM-L's.
- The SCU's DMA does not write to it. The SH-2's DMA and the CPU do.
- A change of the system clock loses its contents, and the cartridge is to be set up again
  after it. hx sets it up in startup alone.

## One run of 1 Mbyte

> [!WARNING]
> Experimental. Sega's manual gives the 1 Mbyte cartridge as two blocks and no more, so this
> holds on some cartridges and not on others.

```make
HX_CART_JOIN:= 1
```

On some cartridges the first block shows again just below the second, which makes the two one
run of 1 Mbyte. With `HX_CART_JOIN` as `1`, startup tests for that:

- Where it finds it, the program gets one run of `HX_CART_1MB`.
- Where it does not, the program gets the two runs.

So a program that sets it still handles two runs. The 4 Mbyte cartridge is one run either way.

## More detail

- API: [`<hx/cart.h>`](../api/cart_8h.html).
- [Switches](../reference/switches.md#hx_cart_join): `HX_CART_JOIN`.
- [Disc settings](../reference/disc.md): `HX_IP_PERIPHERALS`.
- [Startup and the linker script](../in-depth/startup.md#the-cartridge): what startup reads and
  writes, and how it tests for the one run.
