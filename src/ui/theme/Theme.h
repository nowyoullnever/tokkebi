#pragma once

#include <array>
#include <cmath>
#include <cstddef>

namespace tokkebi::theme
{
struct Color { int r; int g; int b; int a = 255; };
constexpr bool operator==(Color a, Color b) { return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a; }

// These identifiers, rather than the array order, are the public palette API.
enum class Palette { Rose600, Blue700, Green600, Brown950, Rose300, Plum800, Cyan500, Ice300, Teal900, Violet500, Paper100, Leaf600 };
constexpr std::array<Color, 12> kPalette {{
  {176, 96, 112}, {12, 84, 180}, {24, 144, 120}, {64, 48, 32},
  {240, 168, 180}, {120, 72, 96}, {80, 160, 176}, {168, 204, 228},
  {32, 80, 80}, {160, 128, 160}, {240, 216, 216}, {72, 144, 72}
}};
constexpr Color PaletteColor(Palette value) { return kPalette[static_cast<std::size_t>(value)]; }
constexpr Color Alpha(Color foreground, Color background, int alpha)
{
  return {(foreground.r * alpha + background.r * (255 - alpha)) / 255,
          (foreground.g * alpha + background.g * (255 - alpha)) / 255,
          (foreground.b * alpha + background.b * (255 - alpha)) / 255};
}
inline double Linear(int channel)
{
  const double value = channel / 255.0;
  return value <= .04045 ? value / 12.92 : std::pow((value + .055) / 1.055, 2.4);
}
inline double Luminance(Color color) { return .2126 * Linear(color.r) + .7152 * Linear(color.g) + .0722 * Linear(color.b); }
inline double Contrast(Color foreground, Color background)
{
  const double a = Luminance(foreground);
  const double b = Luminance(background);
  return (std::max(a, b) + .05) / (std::min(a, b) + .05);
}

enum class ThemeMode { Dark, Light };
enum class FontRole { Heading, Navigation, Body, Caption, Technical, Status };
struct Typography { const char* fontID; float size; Color color; };

struct Tokens {
  Color app, primary, secondary, raised, recessed, audio, overlay;
  Color textPrimary, textSecondary, textMuted, textInverse, textAccent, textDisabled;
  Color borderDefault, borderSubtle, borderStrong, borderFocus, separator;
  Color actionDefault, actionHover, actionPressed, actionDisabled, actionText, navigationHover, navigationHoverText;
  Color selectionFill, selectionBorder, selectionText, focusRing;
  Color ready, success, warning, error, busy, inactive;
  Color waveBackground, wavePrimary, waveSelection, wavePlayhead, waveMarker, technicalOnAudio;
};

constexpr Tokens Dark()
{
  return {PaletteColor(Palette::Brown950), PaletteColor(Palette::Brown950), PaletteColor(Palette::Plum800),
          PaletteColor(Palette::Teal900), PaletteColor(Palette::Brown950), PaletteColor(Palette::Teal900), PaletteColor(Palette::Plum800),
          PaletteColor(Palette::Paper100), PaletteColor(Palette::Ice300), PaletteColor(Palette::Rose300), PaletteColor(Palette::Brown950), PaletteColor(Palette::Rose300), PaletteColor(Palette::Violet500),
          PaletteColor(Palette::Plum800), PaletteColor(Palette::Violet500), PaletteColor(Palette::Rose600), PaletteColor(Palette::Ice300), PaletteColor(Palette::Plum800),
          PaletteColor(Palette::Blue700), PaletteColor(Palette::Cyan500), PaletteColor(Palette::Violet500), PaletteColor(Palette::Rose300), PaletteColor(Palette::Paper100), PaletteColor(Palette::Rose300), PaletteColor(Palette::Brown950),
          PaletteColor(Palette::Rose300), PaletteColor(Palette::Rose600), PaletteColor(Palette::Brown950), PaletteColor(Palette::Ice300),
          PaletteColor(Palette::Green600), PaletteColor(Palette::Leaf600), PaletteColor(Palette::Rose300), PaletteColor(Palette::Rose600), PaletteColor(Palette::Cyan500), PaletteColor(Palette::Violet500),
          PaletteColor(Palette::Teal900), PaletteColor(Palette::Cyan500), PaletteColor(Palette::Rose300), PaletteColor(Palette::Blue700), PaletteColor(Palette::Ice300), PaletteColor(Palette::Paper100)};
}

constexpr Tokens Light()
{
  return {PaletteColor(Palette::Paper100), PaletteColor(Palette::Paper100), PaletteColor(Palette::Ice300),
          PaletteColor(Palette::Rose300), PaletteColor(Palette::Paper100), PaletteColor(Palette::Teal900), PaletteColor(Palette::Plum800),
          PaletteColor(Palette::Brown950), PaletteColor(Palette::Brown950), PaletteColor(Palette::Plum800), PaletteColor(Palette::Paper100), PaletteColor(Palette::Blue700), PaletteColor(Palette::Violet500),
          PaletteColor(Palette::Plum800), PaletteColor(Palette::Violet500), PaletteColor(Palette::Rose600), PaletteColor(Palette::Blue700), PaletteColor(Palette::Plum800),
          PaletteColor(Palette::Blue700), PaletteColor(Palette::Rose600), PaletteColor(Palette::Violet500), PaletteColor(Palette::Rose300), PaletteColor(Palette::Paper100), PaletteColor(Palette::Ice300), PaletteColor(Palette::Brown950),
          PaletteColor(Palette::Rose300), PaletteColor(Palette::Rose600), PaletteColor(Palette::Brown950), PaletteColor(Palette::Blue700),
          PaletteColor(Palette::Green600), PaletteColor(Palette::Leaf600), PaletteColor(Palette::Rose600), PaletteColor(Palette::Rose600), PaletteColor(Palette::Cyan500), PaletteColor(Palette::Violet500),
          PaletteColor(Palette::Teal900), PaletteColor(Palette::Cyan500), PaletteColor(Palette::Rose300), PaletteColor(Palette::Blue700), PaletteColor(Palette::Ice300), PaletteColor(Palette::Paper100)};
}

constexpr Tokens Get(ThemeMode mode) { return mode == ThemeMode::Dark ? Dark() : Light(); }
constexpr std::array<float, 6> kSpacing {{4.f, 8.f, 12.f, 16.f, 24.f, 32.f}};
constexpr float kBorderThin = 1.f, kBorderStandard = 2.f, kBorderStructural = 4.f;
constexpr int kStandaloneWidth = 1160, kStandaloneHeight = 760, kPluginWidth = 1024, kPluginHeight = 680;
constexpr int kPrototypeMinimumWidth = 760, kPrototypeMinimumHeight = 540, kHeaderHeight = 52, kTabRailHeight = 42, kStatusBarHeight = 26;

inline Typography Type(FontRole role, ThemeMode mode)
{
  const auto t = Get(mode);
  switch (role) {
    case FontRole::Heading: return {"tokkebi-primary", 24.f, t.textPrimary};
    case FontRole::Navigation: return {"tokkebi-primary", 14.f, t.textPrimary};
    case FontRole::Body: return {"tokkebi-body", 14.f, t.textPrimary};
    case FontRole::Caption: return {"tokkebi-body", 13.f, t.textSecondary};
    case FontRole::Technical: return {"tokkebi-technical", 13.f, t.technicalOnAudio};
    case FontRole::Status: return {"tokkebi-technical", 13.f, t.textPrimary};
  }
  return {"tokkebi-body", 14.f, t.textPrimary};
}
}
