# Face enrollment script for J.A.R.V.I.S.
# Run from project root. Requires jarvis_enroll to be built.
# Usage: .\scripts\enroll_face.ps1 [label]
# Default label: "owner"

param(
    [string]$Label = "owner"
)

$ErrorActionPreference = "Stop"
$jarvisRoot = Split-Path -Parent $PSScriptRoot
if (-not $jarvisRoot) { $jarvisRoot = (Get-Location).Path }

$enrollExe = Join-Path $jarvisRoot "build\jarvis_enroll.exe"
if (-not (Test-Path $enrollExe)) {
    $enrollExe = Join-Path $jarvisRoot "build\Debug\jarvis_enroll.exe"
}
if (-not (Test-Path $enrollExe)) {
    $enrollExe = Join-Path $jarvisRoot "build\Release\jarvis_enroll.exe"
}

if (-not (Test-Path $enrollExe)) {
    Write-Host "jarvis_enroll not found. Build the project first:"
    Write-Host "  cmake -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake"
    Write-Host "  cmake --build build"
    exit 1
}

$buildDir = Split-Path -Parent $enrollExe
Push-Location $buildDir
try {
    & $enrollExe $Label
} finally {
    Pop-Location
}
