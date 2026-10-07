# Themes (P01.7)

`src/ui/theme/Theme.h` is the one authoritative source of palette values and semantic tokens. A component retrieves `Tokens` from its local `ThemeMode`; ordinary rendering code must not choose raw palette colors or hexadecimal literals.

P01.7 components use the existing action, selection, focus, success, warning, error, busy, disabled and text tokens. Project-rendered labels use the bundled font IDs; modal, input and list state is distinguished by text and borders as well as color.

## Canonical palette

| Identifier | Hex | Meaning |
|---|---|---|
| `rose.600` | `#B06070` | primary accent, selection border |
| `blue.700` | `#0C54B4` | actions, links, navigation |
| `green.600` | `#189078` | ready/play |
| `brown.950` | `#403020` | dark surface, dark text |
| `rose.300` | `#F0A8B4` | selected region, highlight |
| `plum.800` | `#784860` | secondary panel, rail |
| `cyan.500` | `#50A0B0` | waveform, technical emphasis |
| `ice.300` | `#A8CCE4` | secondary highlight |
| `teal.900` | `#205050` | audio surface |
| `violet.500` | `#A080A0` | metadata, auxiliary label |
| `paper.100` | `#F0D8D8` | light surface, description |
| `leaf.600` | `#489048` | saved/completed state |

## Semantic mappings

Both themes provide surfaces (`app`, `primary`, `secondary`, `raised`, `recessed`, `audio`, `overlay`), text, borders, interaction, status and future waveform roles. Status colors are never sufficient by themselves: a later status widget must provide a text label or icon.

| Role | Dark | Light |
|---|---|---|
| primary/app surface | brown.950 | paper.100 |
| secondary surface | plum.800 | ice.300 |
| raised surface | teal.900 | rose.300 |
| audio surface | teal.900 | teal.900 |
| primary / secondary text | paper.100 / ice.300 | brown.950 / brown.950 |
| action / action label | blue.700 / paper.100 | blue.700 / paper.100 |
| navigation hover / label | rose.300 / brown.950 | ice.300 / brown.950 |
| selection fill / text | rose.300 / brown.950 | rose.300 / brown.950 |
| focus ring | ice.300 | blue.700 |
| technical labels on audio | paper.100 | paper.100 |
| ready / success / warning / error / busy / inactive | green.600 / leaf.600 / rose.300 / rose.600 / cyan.500 / violet.500 | green.600 / leaf.600 / rose.600 / rose.600 / cyan.500 / violet.500 |
| waveform background / primary / selection / playhead / marker | teal.900 / cyan.500 / rose.300 / blue.700 / ice.300 | teal.900 / cyan.500 / rose.300 / blue.700 / ice.300 |

`ThemeMode::Dark` is the default. The P01.6 shell owns an ephemeral local mode. Its explicit header theme control toggles mode and invalidates only the shell; it does not reload fonts, save a preference, create a DAW parameter, or affect the audio callback.

## Contrast validation

`ThemeTests` uses WCAG relative luminance, not RGB distance. The tested normal-text pairs all meet 4.5:1 in both themes: primary text on primary surface (9.34:1), secondary text on main surface (Dark 7.48:1; Light 9.34:1), action label on blue (5.27:1), navigation-hover label (Dark 6.60:1; Light 7.48:1), selected text on rose.300 (6.60:1), focus ring against the primary surface (Dark 7.48:1; Light 5.27:1), and both technical-panel labels on teal.900 (6.68:1). The P01.6 shell maps header/inactive tab text, selected/hovered action labels and technical status text to these same validated semantic pairs. Body text uses `text.primary` and is tested in both modes.

`violet.500` on `plum.800` is intentionally below 4.5:1 and has a negative test: it is only an auxiliary/inactive treatment, never readable body text. Disabled text is likewise documented and tested separately as non-body text. `Alpha()` composites a foreground against its declared background before contrast calculation; the test validates paper at 75% over brown (6.03:1). The executable uses explicit runtime checks and nonzero exits, including in Release with `NDEBUG`; it does not rely on C++ `assert()`.

## Typography and geometry

The only font IDs are the packaged P00.4 resources: `tokkebi-primary` (DungGeunMo+), `tokkebi-body` (RIDIBatang), and `tokkebi-technical` (Galmuri11Bitmap 2.40.4). Roles are heading 24 px, navigation 14 px, body 14 px, caption 13 px, technical 13 px and status 13 px. No operating-system fallback is introduced.

Spacing is 4/8/12/16/24/32 logical px. Borders are sharp: thin 1 px, standard 2 px, structural 4 px. Baseline dimensions are Standalone 1160x760, plugin 1024x680, prototype minimum 760x540, header 52 px, tab rail 42 px and status bar 26 px. Future controls request these logical tokens once and let IGraphics apply the active DPI scale; they must not scale already-scaled coordinates.

## Limits and usage

P01.5 provides only a bounded preview surface with heading, Korean/body/technical font samples, panel and selection region. It is not a settings screen, waveform editor, navigation system or status-widget implementation. Future controls should use `Get(mode).textPrimary`, `separator`, `selectionFill`, `focusRing` and the matching semantic roles rather than direct palette calls.
