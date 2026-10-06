# Build (P00.2.1)

## Prerequisites

- Git with submodule support
- CMake 3.21 or later
- Windows: Visual Studio 2022 Build Tools or Visual Studio 2022 with the **Desktop development with C++** workload
- macOS: Xcode Command Line Tools and CMake

The iPlug2 CMake documentation lists CMake 3.14 as its minimum. This project requires 3.21 for its own configuration.

## Initialize dependencies

```powershell
git submodule update --init --recursive
```

`third_party/iPlug2` is pinned by the Git superproject. Do not replace it with a separately downloaded iPlug2 copy.

## Windows x64 (tested configuration)

Run from the repository root:

```powershell
cmake -S . -B build/windows-x64 -G "Visual Studio 17 2022" -A x64
cmake --build build/windows-x64 --config Debug --target SampleGrabber-app
cmake --build build/windows-x64 --config Release --target SampleGrabber-app
```

The executable paths are separated by configuration:

- Debug: `build/windows-x64/out/Debug/SampleGrabber.exe`
- Release: `build/windows-x64/out/Release/SampleGrabber.exe`

For a clean rebuild, remove only the project build directory and configure again:

```powershell
Remove-Item -Recurse -Force build/windows-x64
```

## macOS (not tested in P00.2.1)

```bash
git submodule update --init --recursive
cmake -S . -B build/macos -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/macos --target SampleGrabber-app
```

The selected iPlug2 default backend is NanoVG: GL2 on Windows and Metal on macOS. `resources/SampleGrabber-macOS-Info.plist` supplies the APP bundle metadata. No VST3 or AU target is configured in P00.2.1.

## Troubleshooting

- `iPlug2 is missing`: run the submodule initialization command above.
- Visual Studio generator unavailable: install the Visual C++ workload, then open a new Developer PowerShell.
- Graphics backend errors: use the platform-default NanoVG backend; P00.2 does not configure Skia or external graphics SDKs.
- A fresh Windows configure downloads WebView2 and WIL with the pinned iPlug2 revision, even though this project uses IGraphics only. This is an upstream CMake limitation documented in `docs/DEPENDENCIES.md`; do not commit its generated `_deps/` directory.
