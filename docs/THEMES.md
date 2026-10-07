# Themes (P01.5)

`src/ui/theme/Theme.h` is the single source for the twelve canonical palette colors and semantic theme tokens. No component should choose a raw palette color directly.

## Modes and tokens

`ThemeMode::Dark` defaults to brown/teal surfaces; `ThemeMode::Light` uses paper/ice surfaces. Both expose surface, text, border, interaction, status, and future waveform tokens. Status widgets must pair their color with a label or icon.

The preview is an ephemeral per-view switch: click its mode label to redraw Light/Dark. It does not reload fonts, persist a setting, affect DAW parameters, or access the audio callback.

## Palette and accessibility

The canonical colors are rose `B06070`/`F0A8B4`, blue `0C54B4`, green `189078`, brown `403020`, plum `784860`, cyan `50A0B0`, ice `A8CCE4`, teal `205050`, violet `A080A0`, paper `F0D8D8`, and leaf `489048`.

`ThemeTests` uses WCAG relative luminance: primary text on the primary surface in each mode is >= 4.5:1. Violet on plum is deliberately asserted below 4.5:1, documenting that muted/disabled combinations are not normal readable text. Alpha composites use `Alpha()` before contrast evaluation.

## Layout and type

Spacing tokens are 4/8/12/16/24/32 logical pixels; borders are 1/2/4 px. Typography roles use only `tokkebi-primary`, `tokkebi-body`, and `tokkebi-technical`, with 24/14/13 px defaults. Future controls must scale logical values once with the active IGraphics DPI scale.
