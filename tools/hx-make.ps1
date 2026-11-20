# Runs make in the hx toolchain image.
#
#   pwsh tools/hx-make.ps1 [-Project <dir>] [-Bios <file>] [-Image <image>] [make's arguments]
#
# The three options come first, by their full names. All from the first other argument on goes
# to make as it is.
#
# In the container:
#   /work       the program's folder, -Project or the current one. make runs here.
#   /hx         this clone, read-only. HX_ROOT names it.
#   /bios.bin   the BIOS, read-only, when -Bios or HX_BIOS on the host gives one. HX_BIOS names it.
#
#   pwsh tools/hx-make.ps1 -Project ..\game -j8
#   pwsh tools/hx-make.ps1 -C /hx/libhxmath headers
#   pwsh tools/hx-make.ps1 -Project out -C /hx/libhxmath HX_OUT=/work

$ErrorActionPreference = "Stop"

$options = @{
    Project = "."
    Bios    = $env:HX_BIOS
    Image   = "hx/toolchain:gcc-16.2.0-binutils-2.47"
}
$rest = @($args | ForEach-Object { [string] $_ })
while ($rest.Count -ge 1 -and $rest[0] -match '^-(Project|Bios|Image)$') {
    if ($rest.Count -lt 2) {
        throw "$($rest[0]) takes a value."
    }
    $options[$Matches[1]] = $rest[1]
    $rest = @($rest | Select-Object -Skip 2)
}
# PowerShell's own end of options, which reaches a script run as a file
if ($rest.Count -ge 1 -and $rest[0] -eq "--") {
    $rest = @($rest | Select-Object -Skip 1)
}

docker image inspect $options.Image *> $null
if ($LASTEXITCODE -ne 0) {
    throw "docker has no image $($options.Image). Build it: docker build --tag $($options.Image) $(Join-Path $PSScriptRoot 'image')"
}

$clone = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..")).Path
$arguments = @(
    "run", "--rm",
    "--mount", "type=bind,source=$((Resolve-Path -LiteralPath $options.Project).Path),target=/work",
    "--mount", "type=bind,source=$clone,target=/hx,readonly",
    "--env", "HX_ROOT=/hx"
)
if ($options.Bios) {
    $arguments += @(
        "--mount", "type=bind,source=$((Resolve-Path -LiteralPath $options.Bios).Path),target=/bios.bin,readonly",
        "--env", "HX_BIOS=/bios.bin"
    )
}

docker @arguments --workdir /work $options.Image make @rest
exit $LASTEXITCODE
