# Architecture (P00.2)

`SampleGrabber-app` is a standalone iPlug2 application built with CMake and C++20.

## Current layout

- `src/app/`: the small shared iPlug2 plugin class, configuration, startup UI layout, and minimal audio callback.
- `third_party/iPlug2/`: pinned upstream framework submodule.
- `docs/`: build, architecture, and dependency records.

## Startup, graphics, and shutdown

iPlug2 owns native standalone window creation and event dispatch. `SampleGrabber` supplies an `IGraphics` layout with a text-free static surface: a `#403020` background, `#B06070` border, and approved-palette regions. No application text or font is rendered.

The standalone close event is handled by iPlug2's native application lifecycle. P00.2 creates no background services, user-data files, network requests, or application-owned threads.

## Audio callback

`ProcessBlock()` performs a bounded channel-by-channel pass-through and writes zero to output channels that have no matching input. It allocates no buffers, accesses no filesystem or network resources, and does not generate audio.

## Deliberately absent

P00.2 does not provide VST3, AU, Helper IPC, acquisition, persistence, playback, waveform processing, fonts, or sample-library features. The shared `src/app` class and iPlug2 format configuration can be reused when those separately approved phases add targets and modules.
