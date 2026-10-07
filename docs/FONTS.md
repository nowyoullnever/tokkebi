# Fonts (P00.4)

Bundled files are verified by `tokkebi.FontAssets`; SHA-256 values are in `assets/fonts/manifest.json`.

| Role | Family / file | Source and license | Local preservation |
|---|---|---|---|
| Primary | DungGeunMo 1.301 / `dunggeunmo.ttf` | [official download page](https://cactus.tistory.com/193), public-domain-based statement; not OFL | Local folder had web EOT/WOFF/WOFF2 only; native TTF acquired from the official page. |
| Body | RIDIBatang 1.0.1 / `ridibatang.otf` | [RIDI](https://ridicorp.com/ridibatang/), SIL OFL 1.1 | Copied byte-for-byte from local `RIDIBatang.otf`. |
| Technical | Galmuri11Bitmap 2.40.4 / `galmuri11-bitmap-regular-2.40.4.ttf` | [pinned v2.40.4 release](https://github.com/quiple/galmuri/releases/tag/v2.40.4), SIL OFL 1.1 | Not present locally; acquired from the pinned release. |

## Archived originals (P00.4.1)

The following owner-provided DungGeunMo web assets were rechecked against the
same official public-domain-based permission statement and copied byte-for-byte
for source preservation only. They are declared in the manifest with
`runtime: false`, are not referenced by CMake resources or `src/app/fonts.h`,
and are never loaded by the application.

| Original local filename | Repository archive path | SHA-256 |
|---|---|---|
| `DungGeunMo WebFont/DungGeunMo.eot` | `assets/fonts/archive/dunggeunmo-webfont/DungGeunMo.eot` | `10270cda6048bc2751a80a395040a47dc3fa3cb196cfa17d33579e81839c2186` |
| `DungGeunMo WebFont/DungGeunMo.woff` | `assets/fonts/archive/dunggeunmo-webfont/DungGeunMo.woff` | `e998de9230715bbe7a44ced2db26696fef9fbd406bd92c67f4c47409b7452911` |
| `DungGeunMo WebFont/DungGeunMo.woff2` | `assets/fonts/archive/dunggeunmo-webfont/DungGeunMo.woff2` | `e6fe6ae958ca8b17261e8354ca63ccf4b1caafb6371f9e130b79817f52c5f939` |

`OPEN-FONT-04` remains unresolved: the accessible local folder contains no identifiable fourth family.

The smoke screen uses only the three IDs declared in `src/app/fonts.h`. On a required-font load failure it logs a diagnostic and renders no replacement system-font text. No glyph-by-glyph fallback engine is introduced: supported coverage is limited to each bundled font's actual cmap; unsupported scripts must remain visibly unsupported until a later approved renderer change.

Text coverage samples include Korean, ASCII, technical punctuation and Latin text. Japanese, compatibility Jamo and full currency/math coverage are file-level follow-up checks, not claimed visual coverage.
