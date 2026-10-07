# Architecture (P01.5)

`tokkebi` is a C++20/iPlug2 project with these target names:

- `tokkebi-app`: Standalone on Windows and macOS.
- `tokkebi-vst3`: VST3 on Windows and macOS.
- `tokkebi-au`: AUv2 on macOS only.

All targets share `src/app/tokkebi.cpp`, `src/app/tokkebi.h` and `src/app/config.h`. No duplicate DSP source or compatibility wrapper is maintained.

## Theme foundation

`src/ui/theme/Theme.h` owns the immutable twelve-color palette, semantic Light/Dark token sets, WCAG contrast helpers, typography roles and logical geometry tokens. Controls retrieve `Tokens` from a local `ThemeMode` and must not place raw brand colors in rendering code. The temporary `ThemePreviewControl` owns its mode locally; switching only invalidates that control, reloads no font resource, changes no DAW parameter or persisted state, and never touches the audio callback.

## Audio and graphics behavior

`APP_API` selects `0-2`; iPlug2 opens no input stream, and the callback writes only silence. This preserves P00.2.1's microphone-monitoring and feedback protection.

VST3 and AUv2 select `2-2`; their callback copies each connected input channel to the corresponding output and zeros unmatched outputs. It performs no allocation, file operation, network operation or blocking work.

## Bundled typography boundary

`src/app/fonts.h` is the sole font-loading interface. It registers the three
bundled binaries with IGraphics by stable IDs: DungGeunMo for primary text,
RIDIBatang for body text, and Galmuri11Bitmap 2.40.4 for technical text. The
P00.4 smoke screen exercises Korean, Latin and technical strings using only
those IDs. A required-font load failure emits a diagnostic and attaches no
replacement system-font text. The Windows resource script embeds these assets
in the APP and VST3 binaries; the CMake `RESOURCES` list packages them for
macOS bundles.

## Product identity

The user-facing product name is lowercase `tokkebi`. The stable iPlug2 IDs are plugin `Tkb1` and manufacturer `NYN1`; the same values are reflected in VST3/AU resource metadata. The application release is `0.1.0` (`0x00000100`).

## Deliberately absent

P00.4 adds no Helper, user-data store, downloader, preview, waveform processing, installer, signing, notarization, host-specific integration or workflow UI redesign. Actual DAW loading remains separate manual validation.
