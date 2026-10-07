#include "ui/theme/Theme.h"

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

using namespace tokkebi::theme;

namespace
{
int failures = 0;

void Check(bool condition, std::string_view description)
{
  if (!condition)
  {
    std::cerr << "ThemeTests failure: " << description << '\n';
    ++failures;
  }
}

void CheckReadable(Color foreground, Color background, std::string_view description)
{
  Check(Contrast(foreground, background) >= 4.5, description);
}

void CheckTheme(ThemeMode mode, std::string_view name)
{
  const auto tokens = Get(mode);
  CheckReadable(tokens.textPrimary, tokens.primary, std::string(name) + ": primary text on primary surface");
  CheckReadable(tokens.textPrimary, tokens.secondary, std::string(name) + ": header text on secondary surface");
  CheckReadable(tokens.textSecondary, tokens.app, std::string(name) + ": secondary text on app surface");
  CheckReadable(Type(FontRole::Body, mode).color, tokens.primary, std::string(name) + ": body text on primary surface");
  CheckReadable(tokens.actionText, tokens.actionDefault, std::string(name) + ": action label");
  CheckReadable(tokens.selectionText, tokens.selectionFill, std::string(name) + ": selection text");
  CheckReadable(tokens.focusRing, tokens.primary, std::string(name) + ": focus ring");
  CheckReadable(tokens.technicalOnAudio, tokens.audio, std::string(name) + ": timecode on audio surface");
  CheckReadable(tokens.technicalOnAudio, tokens.audio, std::string(name) + ": metadata label on audio surface");
  Check(Contrast(tokens.textDisabled, tokens.primary) < 4.5, std::string(name) + ": disabled text remains non-body text");
}

bool IsNegativeMode(int argc, char* argv[])
{
  return argc == 2 && std::string_view(argv[1]) == "--negative";
}
}

int main(int argc, char* argv[])
{
  if (IsNegativeMode(argc, argv))
  {
#if defined(NDEBUG)
    std::cerr << "ThemeTests negative mode: NDEBUG is defined\n";
#else
    std::cerr << "ThemeTests negative mode: NDEBUG is not defined\n";
#endif
    Check(false, "controlled invalid condition must produce a nonzero exit status");
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  Check(kPalette.size() == 12, "palette has exactly twelve colors");
  const std::array<Color, 12> expected {{
    {176, 96, 112}, {12, 84, 180}, {24, 144, 120}, {64, 48, 32},
    {240, 168, 180}, {120, 72, 96}, {80, 160, 176}, {168, 204, 228},
    {32, 80, 80}, {160, 128, 160}, {240, 216, 216}, {72, 144, 72}
  }};
  for (std::size_t index = 0; index < expected.size(); ++index)
    Check(kPalette[index] == expected[index], "palette RGB entry differs from approved value");

  Check(Get(ThemeMode::Dark).app == PaletteColor(Palette::Brown950), "dark app token identity");
  Check(Get(ThemeMode::Light).app == PaletteColor(Palette::Paper100), "light app token identity");
  Check(Type(FontRole::Heading, ThemeMode::Dark).fontID == std::string_view("tokkebi-primary"), "heading font ID");
  Check(Type(FontRole::Body, ThemeMode::Dark).fontID == std::string_view("tokkebi-body"), "body font ID");
  Check(Type(FontRole::Technical, ThemeMode::Dark).fontID == std::string_view("tokkebi-technical"), "technical font ID");
  CheckTheme(ThemeMode::Dark, "dark");
  CheckTheme(ThemeMode::Light, "light");

  Check(Contrast(PaletteColor(Palette::Violet500), PaletteColor(Palette::Plum800)) < 4.5,
        "violet on plum is rejected as normal text");
  const auto composited = Alpha(PaletteColor(Palette::Paper100), PaletteColor(Palette::Brown950), 192);
  Check(composited.a == 255, "alpha composite is opaque");
  CheckReadable(composited, PaletteColor(Palette::Brown950), "paper alpha composite on brown");
  Check(kSpacing[0] == 4.f && kSpacing[5] == 32.f && kBorderThin == 1.f && kBorderStructural == 4.f,
        "spacing and border tokens");
  Check(kStandaloneWidth == 1160 && kStandaloneHeight == 760 && kPluginWidth == 1024 && kPluginHeight == 680,
        "window dimensions");
  return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
