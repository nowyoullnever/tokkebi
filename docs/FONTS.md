# Fonts (P00.4)

Bundled files are verified by `tokkebi.FontAssets`; SHA-256 values are in `assets/fonts/manifest.json`.

| Role | Family / file | Source and license | Local preservation |
|---|---|---|---|
| Primary | DungGeunMo 1.301 / `dunggeunmo.ttf` | [official download page](https://cactus.tistory.com/193), public-domain-based statement; not OFL | Local folder had web EOT/WOFF/WOFF2 only; native TTF acquired from the official page. |
| Body | RIDIBatang 1.0.1 / `ridibatang.otf` | [RIDI](https://ridicorp.com/ridibatang/), SIL OFL 1.1 | Copied byte-for-byte from local `RIDIBatang.otf`. |
| Technical | Galmuri11Bitmap 2.40.4 / `galmuri11-bitmap-regular-2.40.4.ttf` | [pinned v2.40.4 release](https://github.com/quiple/galmuri/releases/tag/v2.40.4), SIL OFL 1.1 | Not present locally; acquired from the pinned release. |

`OPEN-FONT-04` remains unresolved: the accessible local folder contains no identifiable fourth family. The local DungGeunMo web files are inventoried but are not separately bundled because the official native TTF is the application resource.

The smoke screen uses only the three IDs declared in `src/app/fonts.h`. On a required-font load failure it logs a diagnostic and renders no replacement system-font text. No glyph-by-glyph fallback engine is introduced: supported coverage is limited to each bundled font's actual cmap; unsupported scripts must remain visibly unsupported until a later approved renderer change.

Text coverage samples include Korean, ASCII, technical punctuation and Latin text. Japanese, compatibility Jamo and full currency/math coverage are file-level follow-up checks, not claimed visual coverage.
