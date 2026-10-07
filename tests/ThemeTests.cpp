#include "ui/theme/Theme.h"

#include <cassert>
#include <string_view>

using namespace tokkebi::theme;

namespace
{
void CheckReadable(Color foreground, Color background) { assert(Contrast(foreground, background) >= 4.5); }

void CheckTheme(ThemeMode mode)
{
  const auto tokens = Get(mode);
  CheckReadable(tokens.textPrimary, tokens.primary);
  CheckReadable(tokens.textSecondary, tokens.app);
  CheckReadable(Type(FontRole::Body, mode).color, tokens.primary);
  CheckReadable(tokens.actionText, tokens.actionDefault);
  CheckReadable(tokens.selectionText, tokens.selectionFill);
  CheckReadable(tokens.focusRing, tokens.primary);
  CheckReadable(tokens.technicalOnAudio, tokens.audio);
  // Disabled text is intentionally non-body text and is not held to 4.5:1.
  assert(Contrast(tokens.textDisabled, tokens.primary) < 4.5);
}
}

int main()
{
  static_assert(kPalette.size() == 12);
  const std::array<Color, 12> expected {{
    {176, 96, 112}, {12, 84, 180}, {24, 144, 120}, {64, 48, 32},
    {240, 168, 180}, {120, 72, 96}, {80, 160, 176}, {168, 204, 228},
    {32, 80, 80}, {160, 128, 160}, {240, 216, 216}, {72, 144, 72}
  }};
  for (std::size_t index = 0; index < expected.size(); ++index)
    assert(kPalette[index] == expected[index]);

  assert(Get(ThemeMode::Dark).app == PaletteColor(Palette::Brown950));
  assert(Get(ThemeMode::Light).app == PaletteColor(Palette::Paper100));
  assert(Type(FontRole::Heading, ThemeMode::Dark).fontID == std::string_view("tokkebi-primary"));
  assert(Type(FontRole::Body, ThemeMode::Dark).fontID == std::string_view("tokkebi-body"));
  assert(Type(FontRole::Technical, ThemeMode::Dark).fontID == std::string_view("tokkebi-technical"));
  CheckTheme(ThemeMode::Dark);
  CheckTheme(ThemeMode::Light);

  // Violet on plum is deliberately invalid normal text and must stay rejected.
  assert(Contrast(PaletteColor(Palette::Violet500), PaletteColor(Palette::Plum800)) < 4.5);
  const auto composited = Alpha(PaletteColor(Palette::Paper100), PaletteColor(Palette::Brown950), 192);
  assert(composited.a == 255);
  assert(Contrast(composited, PaletteColor(Palette::Brown950)) >= 4.5);
  assert(kSpacing[0] == 4.f && kSpacing[5] == 32.f && kBorderThin == 1.f && kBorderStructural == 4.f);
  assert(kStandaloneWidth == 1160 && kStandaloneHeight == 760 && kPluginWidth == 1024 && kPluginHeight == 680);
}
