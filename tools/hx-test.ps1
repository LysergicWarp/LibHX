# Builds each test disc from nothing, runs it on Beetle Saturn and reads its report.
#
#   pwsh tools/hx-test.ps1 [-Runner <folder>] [-RunnerImage <image>] [-Bios <file>]
#       [-Out <folder>] [-Image <image>] [-Region na|eu|jp] [-Frames <count>] [<test> ...]
#
# A test is a folder tests/<group>/<name> of the clone, named here as <group>/<name>:
# startup/high-stack. With none named, every test runs.
#
#   -Runner        a folder that holds beetle-runner, a program that runs a libretro core
#                  without a window, and mednafen_saturn_libretro.so, the Beetle Saturn core.
#                  HX_RUNNER when it is not given. Neither is part of the SDK. What the script
#                  asks of the runner is below.
#   -RunnerImage   the image the two run in. HX_RUNNER_IMAGE when it is not given.
#   -Bios          the BIOS file. HX_BIOS when it is not given.
#   -Out           where the builds and the results go. out, in the clone, when it is not
#                  given. A test's folder there is emptied first.
#   -Image         the toolchain image
#   -Region        the region the disc runs in. A test is built for it too (HX_TEST_REGION,
#                  build/test.mk).
#   -Frames        the frames a disc gets to leave its report
#
# Each test is built twice, with the clone and the build at other places the second time, and
# the archives, the program and the disc image have to be the same bytes. The first build's
# disc is run until the report's mark is in work RAM. A test passes when its report is whole
# and every row in it passes. A note in it is a value to read, in report.txt: it is no row.
#
# A test may name the picture its run is to show (HX_TEST_SCREEN_LINES or
# HX_TEST_SCREEN_PICTURE, build/test.mk). Its disc is then run again, with no stop, two frames
# past the one the report was found after. That frame's screenshot has to be the picture, pixel
# for pixel.
#
# In -Out: run.txt names the clone's commit, the toolchain image and its checksum, the runner's
# image and the BIOS's checksum. A test's folder holds its two builds (first, second),
# build.log, the runner's result.json and log.txt, and report.txt. With a picture it holds
# screen.bmp too, and the second run's screen.json and screen-log.txt.
#
# The exit code is 0 when every test passes.
#
# The runner is called as
#
#   beetle-runner --core <core> --image <cue> --bios <file> --region <region> --frames <count>
#       --stop-address <address> --stop-ascii <text> --capture <address>:<length> ...
#       --core-option <name>=<value> ...
#
# --core-option sets an option of the core, and is given once for each option a test names
# (HX_TEST_CORE_OPTIONS, build/test.mk): the cartridge in the slot, for one.
#
# It runs the disc until the text is at the address in work RAM, or for the count of frames.
# It exits with 0 when the text was there and with 2 when the frames ran out. It writes one
# JSON object to its output, whose "captures" is a list in the order asked, each with "data":
# the bytes at that address as hexadecimal digits, and whose "framesExecuted" is the count of
# frames it ran. A capture is 4096 bytes at most.
#
# For the screenshot it is called with no stop, and with
#
#       --screenshot <frame>:<file>
#
# It then runs for the count of frames, exits with 0, and writes the picture of that frame,
# counted from 1, to the file: a BMP of 32 bits a pixel, the top row first.
[CmdletBinding(PositionalBinding = $false)]
param(
    [string] $Runner = $env:HX_RUNNER,
    [string] $RunnerImage = $env:HX_RUNNER_IMAGE,
    [string] $Bios = $env:HX_BIOS,
    [string] $Out,
    [string] $Image = "hx/toolchain:gcc-16.2.0-binutils-2.47",
    [ValidateSet("na", "eu", "jp")] [string] $Region = "na",
    [int] $Frames = 3600,
    [Parameter(ValueFromRemainingArguments)] [string[]] $Tests
)

$ErrorActionPreference = "Stop"
# A program that fails is this script's to judge, by its exit code
$PSNativeCommandUseErrorActionPreference = $false

$clone = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..")).Path
if (-not $Out) {
    $Out = Join-Path $clone "out"
}
if (-not $Runner) {
    throw "No runner. Give -Runner or set HX_RUNNER: the folder of beetle-runner and mednafen_saturn_libretro.so."
}
if (-not $RunnerImage) {
    throw "No runner image. Give -RunnerImage or set HX_RUNNER_IMAGE."
}
if (-not $Bios) {
    throw "No BIOS. Give -Bios or set HX_BIOS."
}
$runnerPath = (Resolve-Path -LiteralPath $Runner).Path
$biosPath = (Resolve-Path -LiteralPath $Bios).Path
foreach ($name in "beetle-runner", "mednafen_saturn_libretro.so") {
    if (-not (Test-Path -LiteralPath (Join-Path $runnerPath $name))) {
        throw "$runnerPath holds no $name."
    }
}
foreach ($name in $Image, $RunnerImage) {
    docker image inspect $name *> $null
    if ($LASTEXITCODE -ne 0) {
        throw "docker has no image $name."
    }
}

# The tests: every folder with a Makefile in a group's folder
$found = @(Get-ChildItem -LiteralPath (Join-Path $clone "tests") -Directory | ForEach-Object {
    $group = $_.Name
    Get-ChildItem -LiteralPath $_.FullName -Directory |
        Where-Object { Test-Path -LiteralPath (Join-Path $_.FullName "Makefile") } |
        ForEach-Object { "$group/$($_.Name)" }
} | Sort-Object)
if ($Tests) {
    $Tests = @($Tests | ForEach-Object { $_.Replace("\", "/").Trim("/") })
    foreach ($test in $Tests) {
        if ($found -notcontains $test) {
            throw "No test $test. There are: $($found -join ', ')."
        }
    }
} else {
    $Tests = $found
}
if (-not $Tests) {
    throw "The clone holds no test."
}

New-Item -ItemType Directory -Force $Out | Out-Null
$Out = (Resolve-Path -LiteralPath $Out).Path

# What the results came from. The commit is the clone's own: a copy inside another repository
# has none.
$commit = "not a git clone"
if (Get-Command git -ErrorAction SilentlyContinue) {
    $top = git -C $clone rev-parse --show-toplevel 2> $null
    if ($LASTEXITCODE -eq 0 -and
        [IO.Path]::GetFullPath($top).TrimEnd('\', '/') -eq $clone.TrimEnd('\', '/')) {
        $head = git -C $clone rev-parse HEAD
        $changed = git -C $clone status --porcelain
        $commit = if ($changed) { "$head, with changes not committed" } else { $head }
    }
}
$checksum = docker run --rm $Image sh -c "tail -n 1 /opt/sh2eb-elf/versions.txt" 2> $null
@(
    "LibHX: $commit"
    "toolchain image: $Image $(docker image inspect --format '{{.Id}}' $Image)"
    "toolchain: $checksum"
    "runner image: $RunnerImage $(docker image inspect --format '{{.Id}}' $RunnerImage)"
    "BIOS: sha256:$((Get-FileHash -LiteralPath $biosPath -Algorithm SHA256).Hash.ToLower())"
) | Set-Content -LiteralPath (Join-Path $Out "run.txt")

# The two places a test is built at: the clone, and the folder of the build
$places = @(
    @{ Name = "first"; Clone = "/hx"; Build = "/out" }
    @{ Name = "second"; Clone = "/b/second/place/hx"; Build = "/p/two/deeper" }
)
$jobs = [Environment]::ProcessorCount
$markSize = 8
$captureSize = 4096

# The screenshot against the picture a test names: its width and the colour around it, then
# lines of a count of lines and their runs of pixels from the left, each <pixels>:<colour>. The
# picture sits in the screenshot's middle. A colour is compared by the 5 bits of each of blue,
# green and red.
function Compare-Screen {
    param([string] $Path, [string[]] $Screen)

    if ($Screen[0] -cnotmatch '^(\d+) ([0-9a-f]{4})$') {
        return "the test's picture has no width and border colour: $($Screen[0])"
    }
    $width = [int] $Matches[1]
    $border = [Convert]::ToInt32($Matches[2], 16) -band 0x7FFF
    # A line of the picture is its pixels' colours. Lines that are the same share one list
    $lines = [Collections.Generic.List[int[]]]::new()
    foreach ($text in $Screen | Select-Object -Skip 1) {
        if ($text -cnotmatch '^(\d+)((?: \d+:[0-9a-f]{4})+)$') {
            return "the test's picture holds a line that is no count of lines and runs of <pixels>:<colour>: $text"
        }
        $count = [int] $Matches[1]
        $pixels = [Collections.Generic.List[int]]::new()
        foreach ($run in $Matches[2].Trim().Split(" ")) {
            $length, $color = $run.Split(":")
            $pixels.AddRange([int[]] (@([Convert]::ToInt32($color, 16) -band 0x7FFF) * [int] $length))
        }
        if ($pixels.Count -ne $width) {
            return "the test's picture holds a line of $($pixels.Count) pixels, and its width is ${width}: $text"
        }
        $line = $pixels.ToArray()
        for ($same = 0; $same -lt $count; $same++) {
            $lines.Add($line)
        }
    }

    $bytes = [IO.File]::ReadAllBytes($Path)
    if ($bytes.Length -lt 54 -or $bytes[0] -ne 0x42 -or $bytes[1] -ne 0x4D -or
        [BitConverter]::ToUInt16($bytes, 28) -ne 32 -or [BitConverter]::ToInt32($bytes, 30) -ne 0) {
        return "the screenshot is no 32-bit BMP without compression"
    }
    $data = [BitConverter]::ToInt32($bytes, 10)
    $across = [BitConverter]::ToInt32($bytes, 18)
    $down = -[BitConverter]::ToInt32($bytes, 22)
    if ($down -le 0) {
        return "the screenshot's first row is not its top one"
    }
    if ($across -le 0 -or $data -lt 54 -or $bytes.Length -lt $data + [long] $across * $down * 4) {
        return "the screenshot is shorter than its $across by $down pixels"
    }
    if ($across -lt $width -or $down -lt $lines.Count -or
        ($across - $width) % 2 -or ($down - $lines.Count) % 2) {
        return "the screenshot is $across by $down, and the test's picture $width by $($lines.Count)"
    }
    $left = ($across - $width) / 2
    $top = ($down - $lines.Count) / 2
    for ($y = 0; $y -lt $down; $y++) {
        $inside = $y -ge $top -and $y -lt $top + $lines.Count
        $at = $data + $y * $across * 4
        for ($x = 0; $x -lt $across; $x++) {
            $wanted = if ($inside -and $x -ge $left -and $x -lt $left + $width) { $lines[$y - $top][$x - $left] } else { $border }
            # A pixel is blue, green, red and a byte of nothing
            $got = (([int] $bytes[$at] -shr 3) -shl 10) -bor (([int] $bytes[$at + 1] -shr 3) -shl 5) -bor
                ([int] $bytes[$at + 2] -shr 3)
            if ($got -ne $wanted) {
                return ("the screenshot's column $x of line $y is {0:x4}, and the test names {1:x4}" -f
                    $got, $wanted)
            }
            $at += 4
        }
    }
    return $null
}

function Test-Disc {
    param([string] $Test)

    $group, $name = $Test.Split("/")
    $folder = Join-Path $Out $Test
    if (Test-Path -LiteralPath $folder) {
        Remove-Item -LiteralPath $folder -Recurse -Force
    }
    New-Item -ItemType Directory -Force $folder | Out-Null

    # Built from nothing at each place
    foreach ($place in $places) {
        $build = Join-Path $folder $place.Name
        New-Item -ItemType Directory -Force $build | Out-Null
        $log = docker run --rm `
            --mount "type=bind,source=$clone,target=$($place.Clone),readonly" `
            --mount "type=bind,source=$build,target=$($place.Build)" `
            --mount "type=bind,source=$biosPath,target=/bios.bin,readonly" `
            --workdir "$($place.Clone)/tests/$group/$name" `
            $Image make "--jobs=$jobs" "HX_OUT=$($place.Build)" HX_BIOS=/bios.bin `
            "HX_TEST_REGION=$Region" 2>&1
        $failed = $LASTEXITCODE -ne 0
        $log | ForEach-Object { "$_" } | Add-Content -LiteralPath (Join-Path $folder "build.log")
        if ($failed) {
            # The lines that say why, which other jobs' commands follow in the log
            $why = @($log | ForEach-Object { "$_" } |
                Where-Object { $_ -cmatch 'error|Error|\*\*\*|undefined|\.mk:\d+' })
            if (-not $why) {
                $why = @($log | ForEach-Object { "$_" })
            }
            return "the build failed ($($place.Name), build.log):`n" +
                (($why | Select-Object -Last 12 | ForEach-Object { "    $_" }) -join "`n")
        }
    }

    $first = Join-Path $folder "first"
    $second = Join-Path $folder "second"
    $same = @(Get-ChildItem -LiteralPath $first -File |
        Where-Object { $_.Extension -in ".a", ".elf", ".bin", ".nm", ".iso", ".cue" })
    foreach ($file in $same) {
        $other = Join-Path $second $file.Name
        if (-not (Test-Path -LiteralPath $other) -or
            (Get-FileHash -LiteralPath $file.FullName).Hash -ne (Get-FileHash -LiteralPath $other).Hash) {
            return "$($file.Name) is not the same bytes at the two places"
        }
    }

    # The report's place, from the program's symbols
    $symbol = Get-Content -LiteralPath (Join-Path $first "$name.nm") |
        Where-Object { $_ -match '^([0-9a-f]{8}) ([0-9a-f]{8}) \w _hx_test_report$' } |
        Select-Object -First 1
    if (-not $symbol) {
        return "the program has no report (hx_test_report)"
    }
    $null = $symbol -match '^([0-9a-f]{8}) ([0-9a-f]{8}) '
    $address = [Convert]::ToUInt32($Matches[1], 16)
    $size = [Convert]::ToUInt32($Matches[2], 16)

    # What every run of the disc is given. The test's folder of results is where a run writes
    # its screenshot.
    $run = @(
        "run", "--rm",
        "--mount", "type=bind,source=$runnerPath,target=/runner,readonly",
        "--mount", "type=bind,source=$first,target=/disc,readonly",
        "--mount", "type=bind,source=$biosPath,target=/bios.bin,readonly",
        "--mount", "type=bind,source=$folder,target=/results",
        $RunnerImage, "/runner/beetle-runner",
        "--core", "/runner/mednafen_saturn_libretro.so",
        "--image", "/disc/$name.cue",
        "--bios", "/bios.bin",
        "--region", $Region
    )
    # The core's options the test names, one a line
    foreach ($option in @(Get-Content -LiteralPath (Join-Path $first "$name.options") | Where-Object { $_ })) {
        $run += @("--core-option", $option)
    }

    $arguments = $run + @(
        "--frames", $Frames,
        "--stop-address", ("0x{0:X8}" -f $address),
        "--stop-ascii", "HXREPORT"
    )
    for ($at = 0; $at -lt $size; $at += $captureSize) {
        $arguments += @("--capture", ("0x{0:X8}:{1}" -f ($address + $at), [Math]::Min($captureSize, $size - $at)))
    }
    $json = docker @arguments 2> (Join-Path $folder "log.txt")
    $exitCode = $LASTEXITCODE
    $json | Set-Content -LiteralPath (Join-Path $folder "result.json")
    if ($exitCode -eq 2) {
        return "no report in $Frames frames"
    }
    if ($exitCode -ne 0) {
        return "the runner failed with $exitCode (log.txt)"
    }

    try {
        $result = ($json -join "") | ConvertFrom-Json
        $stopped = [int] $result.framesExecuted
        $bytes = [Convert]::FromHexString((($result.captures | ForEach-Object { $_.data }) -join ""))
    } catch {
        return "the runner's result is not to be read (result.json)"
    }
    if ($bytes.Length -ne $size) {
        return "the runner gave $($bytes.Length) bytes of the report's $size"
    }
    if ([Text.Encoding]::ASCII.GetString($bytes, 0, $markSize) -cne "HXREPORT") {
        return "the report has no mark"
    }
    $end = [Array]::IndexOf($bytes, [byte] 0, $markSize)
    if ($end -lt 0) {
        return "the report has no end"
    }
    $text = [Text.Encoding]::ASCII.GetString($bytes, $markSize, $end - $markSize)
    Set-Content -LiteralPath (Join-Path $folder "report.txt") -Value $text -NoNewline

    # A row passes when its two numbers are the same, and says so itself. A note is a value
    # to read: no row, and not counted
    $lines = @($text.Split("`n") | Where-Object { $_ })
    $rows = @()
    $bad = @()
    $notes = 0
    foreach ($line in $lines) {
        if ($line -cmatch '^(pass|FAIL) \S+ ([0-9a-f]{8}) ([0-9a-f]{8})$') {
            $rows += $line
            $same = $Matches[2] -ceq $Matches[3]
            if ($same -ne ($Matches[1] -ceq "pass")) {
                return "the report holds a row that says other than its numbers do: $line"
            }
            if (-not $same) {
                $bad += $line
            }
        } elseif ($line -cmatch '^note \S+ [0-9a-f]{8}$') {
            $notes++
        }
    }
    $full = $lines -ccontains "full"
    $last = $lines | Select-Object -Last 1
    if ($last -cnotmatch '^end ([0-9a-f]{8}) ([0-9a-f]{8})$') {
        return "the report has no last line"
    }
    $counted = [Convert]::ToUInt32($Matches[1], 16)
    $countedBad = [Convert]::ToUInt32($Matches[2], 16)
    if ($lines.Count -ne $rows.Count + $notes + 1 + [int] $full) {
        return "the report holds a line that is no row and no note"
    }
    if ($full) {
        return "the report is full, with lines left out: it counts $counted rows and holds $($rows.Count)"
    }
    if ($counted -ne $rows.Count -or $countedBad -ne $bad.Count) {
        return "the report counts $counted rows and $countedBad failed, and holds $($rows.Count) and $($bad.Count)"
    }
    if ($counted -eq 0) {
        return "the report has no row"
    }
    if ($bad) {
        return "$($bad.Count) of $counted rows failed:`n" + (($bad | ForEach-Object { "    $_" }) -join "`n")
    }

    # The picture, for a test that names one. The disc is run again, with no stop, to the
    # second frame past the one the report was found after, and that frame is the screenshot.
    $screen = @(Get-Content -LiteralPath (Join-Path $first "$name.screen") | Where-Object { $_ })
    if ($screen) {
        if ($stopped -le 0) {
            return "the runner's result has no count of frames (result.json)"
        }
        $frame = $stopped + 2
        $arguments = $run + @("--frames", $frame, "--screenshot", "${frame}:/results/screen.bmp")
        docker @arguments > (Join-Path $folder "screen.json") 2> (Join-Path $folder "screen-log.txt")
        if ($LASTEXITCODE -ne 0) {
            return "the runner failed with $LASTEXITCODE on the run for the screenshot (screen-log.txt)"
        }
        $shot = Join-Path $folder "screen.bmp"
        if (-not (Test-Path -LiteralPath $shot)) {
            return "the runner left no screenshot"
        }
        $unlike = Compare-Screen $shot $screen
        if ($unlike) {
            return "$unlike (screen.bmp)"
        }
    }
    $script:rowsPassed = $counted
    return $null
}

$failures = 0
foreach ($test in $Tests) {
    $rowsPassed = 0
    $failure = Test-Disc $test
    if ($failure) {
        $failures++
        "FAIL $test`: $failure"
    } else {
        "pass $test`: $rowsPassed rows"
    }
}
if ($failures) {
    "$failures of $($Tests.Count) tests failed. Results in $Out"
    exit 1
}
"$($Tests.Count) tests passed. Results in $Out"
exit 0
