# P02.8.1 local audio load and WAV decoder

`DecodeLocalAudio()` is synchronous only for deterministic tests and worker use. Production INBOX selection calls `AudioLoadCoordinator`, which owns exactly one persistent worker and a latest-wins pending request slot. Workers retain shared coordinator state, never `AppShell`, and do not call IGraphics. The IGraphics display tick polls the mutex-protected completion slot on the GUI thread; an applied result calls `SetDirty(false)`, so Ready/Failed renders without user input. A monotonically increasing request generation discards late results. Shutdown wakes and joins that one worker; an active non-cancellable file read may finish before shutdown returns.

`AudioDocument` uses `Empty`, `Loading`, `Ready`, and `Failed`. Loading/failure clear old metadata. Ready displays detected filename, container, codec, source format, channels, rate, frames, and duration. The picker advertises only `wav wave`.

WAV supports PCM 16/24/32-bit and IEEE float32. AIFF/AIFC and FLAC signatures are recognized but return `UnsupportedFormat`; no decoder dependency is added. RIFF parsing is limited to `8 + declaredSize`; chunks or padding crossing that boundary fail, physical trailing bytes are ignored, and unknown in-container chunks are accepted.

Input is limited to 1 GiB. The raw source buffer is exact-size allocated after size validation and short reads are rejected; raw allocation failure returns `TooLarge`. Checked arithmetic computes `frames * channels * sizeof(float)` before allocation; decoded PCM is capped at **512 MiB** (`kMaximumDecodedBytes`). Overflow, over-budget and allocation failure return `TooLarge`. Worker `bad_alloc`, length, standard and unknown exceptions are converted to structured failures. Zero-frame WAV is valid. Block alignment/data length are validated; byte rate is deliberately not checked for WAV compatibility.

`DecodedPcm::At(frame, channel)` checks both dimensions and its flattened index, then throws `std::out_of_range` on invalid coordinates. `SecondsToFrames()` uses nearest-frame `floor(x + .5)` and returns zero for non-finite, negative, zero-rate, or values at/above `LLONG_MAX`.

The audio tests generate temporary deterministic fixtures for all supported PCM variants, RIFF bounds and unknown chunks, malformed input, allocation arithmetic, accessor bounds, conversion limits, and async normal/stale/destruction behavior.
