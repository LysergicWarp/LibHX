# Register maps

`hw/` holds the register maps the libraries share. They are headers with no archive, for C and
assembly alike.

A map gives three things:

- each register's address as a number (`SH2_FRCH`)
- a struct over the module, for C (`SH2_FRT`)
- the names of the bits (`SH2_FTCSR_OVF`)

## From C

The free-running timer's counter:

```c
#include <hx/hw/sh2/frt.h>

static uint16_t
frt_count(void)
{
        /* The high byte first, then the low one */
        const uint16_t high = (uint16_t)(SH2_FRT->frch << 8);

        return (uint16_t)(high | SH2_FRT->frcl);
}
```

A register is reached at its field's width alone. Its map's comments say which way it goes,
read or written, and what order it asks for.

## From assembly

The same header, in a `.sx` source:

```
#include <hx/hw/sh2/frt.h>

        mov.l   .Lftcsr, r1
        mov.b   @r1, r0
        tst     #SH2_FTCSR_OVF, r0

        .align 2
.Lftcsr:
        .long   SH2_FTCSR
```

## The maps so far

| Header | Module |
| --- | --- |
| [`<hx/hw/sh2/cache.h>`](../api/cache_8h.html) | The SH-2's cache |
| [`<hx/hw/sh2/frt.h>`](../api/frt_8h.html) | The SH-2's free-running timer |
| [`<hx/hw/sh2/divu.h>`](../api/divu_8h.html) | The SH-2's divide unit |
| [`<hx/hw/315/scu.h>`](../api/scu_8h.html) | The SCU |
| [`<hx/hw/315/vdp2.h>`](../api/vdp2_8h.html) | VDP2's registers, without their bits |

`<hx/hw/sh2/...>` is for the SH-2's own modules and `<hx/hw/315/...>` for Sega's chips.

## More detail

- API: [`<hx/hw/map.h>`](../api/map_8h.html) states what every map follows. A map's source page
  holds the comments on its registers and bits.
