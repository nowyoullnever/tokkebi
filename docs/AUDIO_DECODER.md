# P02.8 local audio decoder

`tokkebi-audio-core` owns P02.8 local-file inspection, WAV decoding and the instance-local `AudioDocument` source model. It has no dependency on iPlug2 UI classes, plugin parameters, the audio callback, network services or persistent storage.

## Supported containers

| Container | P02.8 status | Detail |
|---|---|---|
| RIFF/WAVE | supported | PCM 16-bit, PCM 24-bit, PCM 32-bit, and IEEE float 32-bit are decoded to interleaved `float` PCM. |
| AIFF/AIFC | recognized, unsupported | No decoder dependency is added in P02.8. The magic bytes are identified and return `UnsupportedFormat`. |
| FLAC | recognized, unsupported | No FLAC decoder exists in the pinned framework. Adding one is deferred rather than silently claiming support. |

The decoder identifies a container from its bytes, not the filename extension. It rejects missing paths, non-files, oversized input (1 GiB), truncated chunks, absent required chunks, invalid alignment/metadata, unsupported encodings and non-finite float payloads with a structured `DecodeErrorCode` and diagnostic.

## Model and threading boundary

`AudioDocument` transitions only through `Empty`, `Loading`, `Ready`, and `Failed`. It stores `AudioFileInfo`, decoded interleaved PCM, error code and diagnostic per UI instance. `LoadLocalFile()` is currently invoked by the user-interface file-selection callback; it must not be called from `ProcessBlock`. This stage deliberately performs synchronous whole-file decoding and has no preview playback, waveform cache, selection, export, database, state serialization or background job system.

## UI boundary

The INBOX `OPEN LOCAL AUDIO` control opens the platform file chooser for WAV/AIFF/FLAC candidates. A selected WAV is decoded and its frame count/sample rate is shown. AIFF and FLAC are truthfully reported as unsupported. The chosen file is not copied, altered, persisted or registered in a Library.

## Tests

`tokkebi-audio-tests` creates deterministic byte fixtures in its temporary directory, including WAV PCM16, PCM24 and float32 payloads. It tests container inspection independent of extension, UTF-8 path handling, missing/nonregular paths, truncated and corrupt chunks, invalid alignment, unsupported formats, AIFF/FLAC recognition, conversion helpers and all `AudioDocument` terminal states. Fixtures are removed after the test; no user audio is read or modified.
