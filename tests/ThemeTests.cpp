#include "ui/theme/Theme.h"
#include <cassert>
using namespace tokkebi::theme;
int main() {
  static_assert(kPalette.size() == 12);
  assert((PaletteColor(Palette::Rose600) == Color{176,96,112}));
  assert((PaletteColor(Palette::Paper100) == Color{240,216,216}));
  assert(Get(ThemeMode::Dark).app == PaletteColor(Palette::Brown950));
  assert(Get(ThemeMode::Light).app == PaletteColor(Palette::Paper100));
  assert(Contrast(Dark().textPrimary, Dark().primary) >= 4.5);
  assert(Contrast(Light().textPrimary, Light().primary) >= 4.5);
  assert(Contrast(PaletteColor(Palette::Violet500), PaletteColor(Palette::Plum800)) < 4.5);
  assert(Alpha(PaletteColor(Palette::Paper100), PaletteColor(Palette::Brown950), 128).a == 255);
  assert(kSpacing[0] == 4.f && kSpacing[5] == 32.f && kBorderThin == 1.f && kBorderStructural == 4.f);
}
