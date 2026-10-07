[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'

$sdkRepository = 'https://github.com/steinbergmedia/vst3sdk.git'
$sdkRevision = '8b59557d881bb0158ba08ff256b26f025f078314'
$scriptRoot = Split-Path -Parent $PSCommandPath
$repositoryRoot = Split-Path -Parent $scriptRoot
$sdkDirectory = Join-Path $repositoryRoot 'third_party/iPlug2/Dependencies/IPlug/VST3_SDK'
$sdkParent = Split-Path -Parent $sdkDirectory
$revisionMarker = Join-Path $sdkDirectory '.tokkebi-vst3-sdk-revision'

if (Test-Path $revisionMarker) {
  $currentRevision = (Get-Content -Raw $revisionMarker).Trim()
  if ($currentRevision -eq $sdkRevision -and (Test-Path (Join-Path $sdkDirectory 'pluginterfaces/vst/ivstaudioprocessor.h'))) {
    Write-Host "VST3 SDK already pinned at $sdkRevision"
    exit 0
  }

  if ($currentRevision -ne $sdkRevision) {
    throw "VST3 SDK marker at $sdkDirectory is $currentRevision; expected $sdkRevision. Correct that SDK directory deliberately, then run this script again."
  }

  Write-Host "Repairing incomplete VST3 SDK checkout at $sdkDirectory"
}

New-Item -ItemType Directory -Force -Path $sdkParent | Out-Null
$temporaryCheckout = Join-Path ([System.IO.Path]::GetTempPath()) ("tokkebi-vst3sdk-" + [guid]::NewGuid().ToString('N'))

try {
  git clone --no-checkout $sdkRepository $temporaryCheckout
  git -C $temporaryCheckout checkout --detach $sdkRevision
  # iPlug2 compiles only these VST3 SDK submodules. Do not fetch SDK docs,
  # tutorials or VSTGUI merely to build the iPlug2 VST3 wrapper.
  git -C $temporaryCheckout submodule update --init --recursive base pluginterfaces public.sdk

  # iPlug2 tracks a README at the destination as setup guidance. Preserve it;
  # copy only the SDK payload folders and license material required for building.
  foreach ($entry in (Get-ChildItem -Force $temporaryCheckout)) {
    if ($entry.Name -in @('.git', 'README.md')) {
      continue
    }
    Copy-Item -Recurse -Force $entry.FullName $sdkDirectory
  }
  Set-Content -NoNewline -Path $revisionMarker -Value $sdkRevision
}
finally {
  if (Test-Path $temporaryCheckout) {
    Remove-Item -Recurse -Force -LiteralPath $temporaryCheckout
  }
}

$requiredHeader = Join-Path $sdkDirectory 'pluginterfaces/vst/ivstaudioprocessor.h'
if (-not (Test-Path $requiredHeader)) {
  throw "Pinned VST3 SDK checkout is incomplete: $requiredHeader is missing."
}

Write-Host "Installed VST3 SDK revision $sdkRevision at $sdkDirectory"
