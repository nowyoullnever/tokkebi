# Architecture (P00.3)

`tokkebi` is a C++20/iPlug2 project with these target names:

- `tokkebi-app`: Standalone on Windows and macOS.
- `tokkebi-vst3`: VST3 on Windows and macOS.
- `tokkebi-au`: AUv2 on macOS only.

All targets share `src/app/tokkebi.cpp`, `src/app/tokkebi.h` and `src/app/config.h`. No duplicate DSP source or compatibility wrapper is maintained.

## Audio and graphics behavior

`APP_API` selects `0-2`; iPlug2 opens no input stream, and the callback writes only silence. This preserves P00.2.1's microphone-monitoring and feedback protection.

VST3 and AUv2 select `2-2`; their callback copies each connected input channel to the corresponding output and zeros unmatched outputs. It performs no allocation, file operation, network operation or blocking work. The shared IGraphics layout remains text-free and uses only the existing approved palette colors.

## Product identity

The user-facing product name is lowercase `tokkebi`. The stable iPlug2 IDs are plugin `Tkb1` and manufacturer `NYN1`; the same values are reflected in VST3/AU resource metadata. The application release is `0.1.0` (`0x00000100`).

## Deliberately absent

P00.3 adds no Helper, user-data store, downloader, preview, waveform processing, font bundle, installer, signing, notarization, host-specific integration or UI redesign. Actual DAW loading remains separate manual validation.
