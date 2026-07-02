param(
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string]$Config = 'Release'
)

$BuildDir = Join-Path $PSScriptRoot 'build'
$SourceDir = $PSScriptRoot

# Detect platform
$IsLinux = [Environment]::OSVersion.Platform -eq [PlatformID]::Unix
$IsWin = -not $IsLinux

if ($IsLinux) {
    $Generator = 'Unix Makefiles'
} else {
    $VS = Get-ChildItem "C:\Program Files\Microsoft Visual Studio\*\*\MSVC\*\bin\Hostx64\x64\cl.exe" -ErrorAction SilentlyContinue |
          Select-Object -First 1
    if ($VS) {
        $Generator = 'Visual Studio 17 2022'
    } else {
        $Generator = 'MinGW Makefiles'
    }
}

# Auto-detect MinGW prefix for SDL2 on Windows
$ExtraArgs = @()
if ($IsWin) {
    $MinGWPaths = @(
        'C:\msys64\mingw64',
        'C:\msys64\mingw32',
        'C:\msys2\mingw64',
        'C:\msys2\mingw32'
    )
    foreach ($p in $MinGWPaths) {
        if (Test-Path (Join-Path $p 'lib\cmake\SDL2')) {
            $ExtraArgs += "-DCMAKE_PREFIX_PATH=$p"
            break
        }
    }
}

# Clean build dir (from a dir that won't conflict)
Push-Location $env:TEMP
try {
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
    }
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
} finally {
    Pop-Location
}

Push-Location $BuildDir
try {
    cmake -G $Generator -DCMAKE_BUILD_TYPE=$Config @ExtraArgs $SourceDir
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    cmake --build . --config $Config
} finally {
    Pop-Location
}
