# Architecture (P00.2.1)

`SampleGrabber-app` is a standalone iPlug2 application built with CMake and C++20.

## Current layout

- `src/app/`: the small shared iPlug2 plugin class, configuration, startup UI layout, and minimal audio callback.
- `third_party/iPlug2/`: pinned upstream framework submodule.
- `docs/`: build, architecture, and dependency records.

## Startup, graphics, and shutdown

iPlug2 owns native standalone window creation and event dispatch. `SampleGrabber` supplies an `IGraphics` layout with a text-free static surface: a `#403020` background, `#B06070` border, and approved-palette regions. No application text or font is rendered.

The standalone close event is handled by iPlug2's native application lifecycle. P00.2.1 creates no background services, user-data files, network requests, or application-owned threads.

## Audio callback

For the Standalone target, `APP_API` selects a `0-2` topology. iPlug2 therefore opens no audio input stream, and `ProcessBlock()` writes silence to each output frame. This avoids default live microphone monitoring and feedback risk during the bootstrap.

For a future non-APP plugin target, the same configuration selects `2-2` and retains bounded transparent pass-through. Both branches allocate no buffers and access neither filesystem nor network resources in the callback.

## Deliberately absent

P00.2.1 does not provide VST3, AU, Helper IPC, acquisition, persistence, playback, waveform processing, fonts, or sample-library features. The shared `src/app` class and iPlug2 format configuration can be reused when those separately approved phases add targets and modules.
