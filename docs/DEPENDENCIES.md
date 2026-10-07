# Dependencies (P00.3)

## iPlug2

- Repository: <https://github.com/iPlug2/iPlug2>
- Pinned revision: `d54f69050f517e43b941d88c2a170f0a840b9ee4`
- Integration: Git submodule at `third_party/iPlug2`
- License: iPlug2 zlib-style license in `third_party/iPlug2/LICENSE.txt`

## VST3 SDK

- Repository: <https://github.com/steinbergmedia/vst3sdk>
- Version/tag: `v3.7.13_build_42`
- Pinned commit: `8b59557d881bb0158ba08ff256b26f025f078314`
- License: Steinberg VST 3 SDK License, supplied in the SDK checkout; review it before redistribution.
- Initialization: `pwsh -File scripts/setup-vst3-sdk.ps1`
- Required iPlug2 path: `third_party/iPlug2/Dependencies/IPlug/VST3_SDK`

The SDK is not a Git submodule because the pinned iPlug2 CMake modules require it inside the iPlug2 dependency tree. The initialization script checks out the exact commit there. The directory and all generated SDK outputs are ignored; CI recreates it from the recorded source and revision.

## Graphics

The project uses iPlug2 IGraphics with its default NanoVG backend: GL2 on Windows and Metal on macOS. No graphics assets or fonts are bundled in P00.3.

## WebView2/WIL configure-time limitation

The pinned iPlug2 revision unconditionally creates `iPlug2::Extras::IWebViewControl` while `find_package(iPlug2 REQUIRED)` loads. On Windows it fetches WIL and downloads WebView2 even though `tokkebi` uses IGraphics only.

- WIL repository: `https://github.com/microsoft/wil.git`
- WIL revision: `v1.0.240803.1`
- WebView2 version: `1.0.2903.40`
- WebView2 endpoint: `https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/1.0.2903.40`

P00.3 does not patch iPlug2, change its pin or commit generated `_deps/` files. GitHub Actions leaves fetching enabled and reports configuration failures directly.
