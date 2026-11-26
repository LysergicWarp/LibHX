# Fixed-point math

libhxmath is fixed-point math for the SH-2: 16.16 arithmetic, trig, vectors and matrices.
`<hx/math.h>` takes everything in.

```c
#include <hx/math.h>

int32_t
example(void)
{
        const hx_fix16_t half = HX_FIX16(0.5);
        const hx_fix16_t product = hx_mul(HX_FIX16(3), half);   /* 1.5 */
        const hx_fix14_t sine = hx_sin(HX_DEG(90));             /* 1 */

        return hx_fix16_to_int(product) + sine;
}
```

## Types

| Type | Holds | Literal |
| --- | --- | --- |
| `hx_fix16_t` | 16.16, the type of most values | `HX_FIX16(1.5)` |
| `hx_fix14_t` | 2.14, a sine or a matrix's rotation | `HX_FIX14(0.5)` |
| `hx_fix2_t` | 14.2, a model's coordinate | `HX_FIX2(10)` |
| `hx_angle_t` | An angle, 65536 a turn | `HX_DEG(90)` |

The number in a fixed-point name is its fraction bits. The vectors are `hx_vec2_t` and
`hx_vec3_t`, a model's point is `hx_vtx_t`, and a matrix is `hx_mat_t`: a rotation and a
translation. A sine with its cosine is `hx_sincos_t`.

## Conventions

- x right, y down, z into the screen.
- A positive angle turns x toward y, y toward z, z toward x.
- The result is the last argument. An aggregate comes back through it, a scalar by value.

## Names

A function's name says how it rounds and what it gives up:

| Name | What |
| --- | --- |
| `hx_mul` | The plain name: accurate, rounds down, wraps |
| `hx_mul_round` | Rounds to the nearest |
| `hx_sqrt_fast` | Gives up accuracy for speed |

- A plain name drops the places it cannot keep. A product or a shift rounds down, a divide
  rounds toward zero.
- The sine, cosine, tangent, atan2 and the rotation `hx_mat_lookat` builds are to the nearest
  under their plain names.

## A view and its points

A camera's view, and a model's points onto a screen 352 pixels wide:

```c
#include <hx/math.h>

static const hx_vtx_t model[3] = {
        { HX_FIX2(-10), HX_FIX2(10), HX_FIX2(0) },
        { HX_FIX2(10), HX_FIX2(10), HX_FIX2(0) },
        { HX_FIX2(0), HX_FIX2(-10), HX_FIX2(0) },
};

static hx_projected_t points[3];

void
project(void)
{
        const hx_vec3_t eye = { HX_FIX16(0), HX_FIX16(0), HX_FIX16(-100) };
        const hx_vec3_t target = { HX_FIX16(0), HX_FIX16(0), HX_FIX16(0) };
        hx_mat_t view;

        hx_mat_lookat(&eye, &target, 0, &view);
        hx_mat_project_batch(&view, model, 3, hx_view_distance(352, HX_DEG(60)), points);
}
```

Each of `points` is a pixel from the screen's centre and the point's z in view space.

## Divides

The divides use the SH-2's divide unit, which holds one divide at a time. A divide can run
while the program does other work:

```c
hx_div_start(a, b);
/* Other work, with no divide in it */
hx_fix16_t quotient = hx_div_collect();
```

> [!NOTE]
> A function that uses the divide unit says so in its comment. None of them is for an interrupt
> handler.

## More detail

- API: [`<hx/math.h>`](../api/math_8h.html) states every convention.
  [`types.h`](../api/types_8h.html), [`fix16.h`](../api/fix16_8h.html),
  [`trig.h`](../api/trig_8h.html), [`vec2.h`](../api/vec2_8h.html),
  [`vec3.h`](../api/vec3_8h.html) and [`mat.h`](../api/mat_8h.html) give each function's
  accuracy and range.
- [Inside a library](../in-depth/library.md): the assembly, its C references, the header check.
