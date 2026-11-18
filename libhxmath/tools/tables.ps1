# Writes the library's tables into ..\src, beside its sources: sincos_table.c, tan_table.c,
# atan_table.c, sqrt_table.c, rsqrt_table.c.
$ErrorActionPreference = "Stop"
[cultureinfo]::CurrentCulture = [cultureinfo]::InvariantCulture

$source = (Resolve-Path (Join-Path $PSScriptRoot "..\src")).Path
$note = "/* Written by tools/tables.ps1 */"

function Rounded([double] $value) {
    [int][Math]::Round($value, [MidpointRounding]::AwayFromZero)
}

function Rows([string[]] $items, [int] $perRow) {
    for ($first = 0; $first -lt $items.Length; $first += $perRow) {
        $last = [Math]::Min($first + $perRow, $items.Length) - 1
        $comma = if ($last -lt ($items.Length - 1)) { "," } else { "" }
        "        " + ($items[$first..$last] -join ", ") + $comma
    }
}

function Save([string] $name, [string[]] $lines) {
    [System.IO.File]::WriteAllText((Join-Path $source $name), (($lines -join "`n") + "`n"))
}

# Sine and cosine side by side, 4096 steps a turn, 2.14
$pairs = for ($i = 0; $i -lt 4096; $i++) {
    $angle = 2.0 * [Math]::PI * $i / 4096.0
    "{{ {0,6}, {1,6} }}" -f (Rounded ([Math]::Sin($angle) * 16384.0)), (Rounded ([Math]::Cos($angle) * 16384.0))
}
Save "sincos_table.c" (@(
    $note
    ""
    "#include <hx/trig.h>"
    ""
    "const hx_fix14_t hx_priv_sincos[4096][2] __attribute__((aligned(16))) = {"
) + (Rows $pairs 4) + @("};"))

# The tangent over half a turn in 1024 steps and the first again, 16.16. The quarter turn has no
# tangent: its entry carries the line of the two below it on
$tangents = for ($i = 0; $i -le 1024; $i++) {
    if ($i -eq 512) { 0 } else { Rounded ([Math]::Tan([Math]::PI * ($i % 1024) / 1024.0) * 65536.0) }
}
$tangents[512] = (2 * $tangents[511]) - $tangents[510]
Save "tan_table.c" (@(
    $note
    ""
    "#include <hx/trig.h>"
    ""
    "const hx_fix16_t hx_priv_tan[1025] __attribute__((aligned(16))) = {"
) + (Rows ($tangents | ForEach-Object { "{0,9}" -f $_ }) 8) + @("};"))

# The arctangent of 0 to 1 in 1024 steps, as an angle
$arcs = for ($i = 0; $i -le 1024; $i++) {
    "{0,4}" -f (Rounded ([Math]::Atan($i / 1024.0) * 32768.0 / [Math]::PI))
}
Save "atan_table.c" (@(
    $note
    ""
    "#include ""tables.h"""
    ""
    "const uint16_t hx_priv_arcs[1025] __attribute__((aligned(16))) = {"
) + (Rows $arcs 12) + @("};"))

# The root of a value by its top 8 bits, 0 to 255, by 4096, rounded down, each with the step to
# the next. hx_sqrt reads it from 64 up, hx_sqrt_fast all of it
function Root([int] $top) {
    [Math]::Min([int][Math]::Floor([Math]::Sqrt($top) * 4096.0), 65535)
}
$roots = for ($i = 0; $i -le 255; $i++) {
    "{{ {0,5}, {1,4} }}" -f (Root $i), ((Root ($i + 1)) - (Root $i))
}
Save "sqrt_table.c" (@(
    $note
    ""
    "#include <hx/fix16.h>"
    ""
    "const uint16_t hx_priv_roots[256][2] __attribute__((aligned(16))) = {"
) + (Rows $roots 6) + @("};"))

# One over the root of the same by its top 8 bits, as a share of 65536 rounded down and held
# under it, each with the step down to the next
function ReciprocalRoot([int] $top) {
    [Math]::Min([int][Math]::Floor(524288.0 / [Math]::Sqrt($top)), 65535)
}
$reciprocalRoots = for ($i = 64; $i -le 255; $i++) {
    "{{ {0,5}, {1,3} }}" -f (ReciprocalRoot $i), ((ReciprocalRoot $i) - (ReciprocalRoot ($i + 1)))
}
Save "rsqrt_table.c" (@(
    $note
    ""
    "#include ""tables.h"""
    ""
    "const uint16_t hx_priv_reciprocal_roots[192][2] __attribute__((aligned(16))) = {"
) + (Rows $reciprocalRoots 6) + @("};"))
