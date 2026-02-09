# J.A.R.V.I.S. Setup Script
# Downloads models and prepares the environment.
# Run from project root.

param(
    [switch]$SkipBuild,
    [switch]$SkipModels
)

$ErrorActionPreference = "Stop"
$jarvisRoot = (Get-Location).Path
if ($PSScriptRoot) {
    $jarvisRoot = Split-Path -Parent $PSScriptRoot
}

Push-Location $jarvisRoot

# Create directories
$dirs = @(
    "models\whisper",
    "models\llm",
    "models\tts",
    "models\face",
    "assets\audio",
    "assets\face_db"
)
foreach ($d in $dirs) {
    New-Item -ItemType Directory -Force -Path $d | Out-Null
}

if (-not $SkipModels) {
    Write-Host "Downloading face recognition models..."

    $faceDir = Join-Path $jarvisRoot "models\face"
    $landmarksUrl = "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2"
    $resnetUrl = "http://dlib.net/files/dlib_face_recognition_resnet_model_v1.dat.bz2"

    function Download-AndExtract {
        param([string]$Url, [string]$OutDir)
        $filename = [System.IO.Path]::GetFileName($Url)
        $outPath = Join-Path $OutDir $filename
        if (Test-Path ($outPath -replace '\.bz2$','')) {
            Write-Host "  Already have $filename (extracted)"
            return
        }
        Write-Host "  Downloading $filename..."
        Invoke-WebRequest -Uri $Url -OutFile $outPath -UseBasicParsing
        Write-Host "  Extracting..."
        & 7z x $outPath -o"$OutDir" -y 2>$null
        if ($LASTEXITCODE -ne 0) {
            $bzip = Get-Command bzip2 -ErrorAction SilentlyContinue
            if ($bzip) {
                & bzip2 -d $outPath
            } else {
                Write-Host "  Install 7-Zip or bzip2 to extract. Or extract $outPath manually."
            }
        }
        Remove-Item $outPath -ErrorAction SilentlyContinue
    }

    try {
        Download-AndExtract -Url $landmarksUrl -OutDir $faceDir
        Download-AndExtract -Url $resnetUrl -OutDir $faceDir
    } catch {
        Write-Host "Download failed: $_"
        Write-Host "Manual download:"
        Write-Host "  1. shape_predictor_68_face_landmarks.dat from http://dlib.net/files/"
        Write-Host "  2. dlib_face_recognition_resnet_model_v1.dat from http://dlib.net/files/"
        Write-Host "  Place both in models/face/"
    }

    Write-Host ""
    Write-Host "Whisper and LLM models are large. Download manually:"
    Write-Host "  whisper: https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-base.en.bin -> models/whisper/"
    Write-Host "  llm: Qwen2.5-7B-Instruct Q4_K_M from HuggingFace -> models/llm/"
    Write-Host "  TTS: Piper en_GB-alan-medium from https://huggingface.co/rhasspy/piper-voices -> models/tts/"
}

if (-not $SkipBuild) {
    Write-Host ""
    Write-Host "Building J.A.R.V.I.S..."

    if (-not (Test-Path "vcpkg")) {
        Write-Host "Cloning vcpkg..."
        git clone https://github.com/microsoft/vcpkg.git
        .\vcpkg\bootstrap-vcpkg.bat -disableMetrics
    }

    $cmake = Get-Command cmake -ErrorAction SilentlyContinue
    if (-not $cmake) {
        $cmakePath = "C:\Program Files\CMake\bin\cmake.exe"
        if (Test-Path $cmakePath) { $env:PATH = "C:\Program Files\CMake\bin;$env:PATH" }
    }

    cmake -B build -DCMAKE_TOOLCHAIN_FILE="$jarvisRoot\vcpkg\scripts\buildsystems\vcpkg.cmake"
    cmake --build build --config Release
}

Write-Host ""
Write-Host "Setup complete. Next steps:"
Write-Host "  1. Place 'Should I Stay or Should I Go.mp3' in assets/audio/should_i_stay.mp3"
Write-Host "  2. Run: .\build\jarvis_enroll.exe  (or jarvis_enroll from build dir) to enroll your face"
Write-Host "  3. Run: .\build\jarvis.exe  (from build dir)"

Pop-Location
