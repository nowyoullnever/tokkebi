# P02.9 waveform foundation

Peak cache buckets hold per-channel minimum, maximum, and half-open source-frame ranges. Level zero uses 64 frames; higher levels aggregate pairs and preserve extrema and final partial buckets. The immutable cache is built by one persistent latest-request worker from a shared immutable PCM snapshot, never by copying the interleaved vector on the UI thread. A new source invalidates cache state immediately; generation matching drops stale work and worker failures retain diagnostics.

Viewports use source frames, map the visible range to pixels, support pointer-centred wheel zoom, Shift+wheel scroll and `F` full view. Rendering selects a cache level and binary-searches visible buckets rather than iterating PCM or the full level. Mono has one labelled lane; stereo keeps independent L/R lanes, zero lines, semantic `waveBackground`/`wavePrimary` tokens, and a computed time ruler. Playback, playhead, selection, IN/OUT, trim, and export remain absent.
