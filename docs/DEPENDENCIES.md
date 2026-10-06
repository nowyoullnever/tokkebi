# Dependencies (P00.2.1)

## iPlug2

- Repository: <https://github.com/iPlug2/iPlug2>
- Pinned revision: `d54f69050f517e43b941d88c2a170f0a840b9ee4`
- Integration: Git submodule at `third_party/iPlug2`
- Initialization: `git submodule update --init --recursive`
- License: iPlug2's zlib-style license in `third_party/iPlug2/LICENSE.txt`. Its bundled third-party components have their own notices; see that file before redistribution.

## Graphics

P00.2 uses iPlug2 IGraphics with the framework-default NanoVG backend. The documented defaults are GL2 on Windows and Metal on macOS. Skia is not enabled and no graphics assets are bundled.

## WebView2/WIL configure-time limitation

The pinned iPlug2 revision unconditionally creates `iPlug2::Extras::IWebViewControl` in `Scripts/cmake/IPlug.cmake` when `find_package(iPlug2 REQUIRED)` loads the framework. On Windows that block uses CMake `FetchContent` to clone WIL and downloads the WebView2 NuGet SDK even when the consuming target is IGraphics-only.

- WIL repository: `https://github.com/microsoft/wil.git`
- WIL revision: `v1.0.240803.1`
- WebView2 version: `1.0.2903.40`
- WebView2 endpoint: `https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/1.0.2903.40`

This iPlug2 revision exposes no supported option to disable that block. P00.2.1 deliberately does not patch the upstream submodule, alter its pin, or commit generated `_deps/` files. A future upstream option or separately reviewed, maintained upstream patch is required before this download can be removed safely.

## Required SDKs

The Standalone-only target in P00.2.1 requires the native compiler toolchain described in `docs/BUILD.md`. It does not require the VST3, AU, AAX, CLAP, FFmpeg, yt-dlp, SQLite, or any P2P SDK.
