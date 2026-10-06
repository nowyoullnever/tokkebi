# Dependencies (P00.2)

## iPlug2

- Repository: <https://github.com/iPlug2/iPlug2>
- Pinned revision: `d54f69050f517e43b941d88c2a170f0a840b9ee4`
- Integration: Git submodule at `third_party/iPlug2`
- Initialization: `git submodule update --init --recursive`
- License: iPlug2's zlib-style license in `third_party/iPlug2/LICENSE.txt`. Its bundled third-party components have their own notices; see that file before redistribution.

## Graphics

P00.2 uses iPlug2 IGraphics with the framework-default NanoVG backend. The documented defaults are GL2 on Windows and Metal on macOS. Skia is not enabled and no graphics assets are bundled.

## Required SDKs

The Standalone-only target in P00.2 requires the native compiler toolchain described in `docs/BUILD.md`. It does not require the VST3, AU, AAX, CLAP, FFmpeg, yt-dlp, SQLite, or any P2P SDK.
