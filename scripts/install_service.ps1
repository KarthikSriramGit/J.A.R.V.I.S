# J.A.R.V.I.S. Windows Service Installation
# Run as Administrator.
# Usage: .\scripts\install_service.ps1

param(
    [switch]$Uninstall
)

$ErrorActionPreference = "Stop"
$jarvisRoot = Split-Path -Parent $PSScriptRoot
$exePath = Join-Path $jarvisRoot "build\Release\jarvis.exe"
if (-not (Test-Path $exePath)) {
    $exePath = Join-Path $jarvisRoot "build\jarvis.exe"
}

if (-not (Test-Path $exePath)) {
    Write-Host "jarvis.exe not found. Build the project first."
    exit 1
}

$serviceName = "JARVIS"
$displayName = "J.A.R.V.I.S. AI Assistant"

if ($Uninstall) {
    Write-Host "Stopping and removing J.A.R.V.I.S. service..."
    Stop-Service -Name $serviceName -ErrorAction SilentlyContinue
    sc.exe delete $serviceName
    Write-Host "Service removed."
} else {
    Write-Host "Installing J.A.R.V.I.S. as Windows service..."
    $binPath = "`"$exePath`""
    New-Service -Name $serviceName -DisplayName $displayName -BinaryPathName $binPath -StartupType Automatic
    Write-Host "Service installed. Start with: Start-Service $serviceName"
}
