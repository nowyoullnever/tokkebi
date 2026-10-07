#pragma once

#include "../theme/Theme.h"

namespace tokkebi::ui
{
struct Rect
{
  float x = 0.f;
  float y = 0.f;
  float width = 0.f;
  float height = 0.f;
  constexpr bool Contains(float pointX, float pointY) const { return pointX >= x && pointX < x + width && pointY >= y && pointY < y + height; }
};

enum class WidthClass { Compact, Intermediate, Wide };
struct ShellLayout
{
  Rect bounds;
  Rect header;
  Rect tabRail;
  Rect content;
  Rect statusBar;
  Rect contentPadding;
  WidthClass widthClass = WidthClass::Compact;
  bool compactTabs = false;
};

constexpr float ClampNonNegative(float value) { return value < 0.f ? 0.f : value; }
constexpr ShellLayout CalculateShellLayout(float width, float height)
{
  const float safeWidth = ClampNonNegative(width);
  const float safeHeight = ClampNonNegative(height);
  const bool compact = safeWidth < static_cast<float>(theme::kPrototypeMinimumWidth);
  const float headerHeight = safeHeight < theme::kHeaderHeight ? safeHeight : static_cast<float>(theme::kHeaderHeight);
  const float afterHeader = safeHeight - headerHeight;
  const float statusHeight = afterHeader < theme::kStatusBarHeight ? afterHeader : static_cast<float>(theme::kStatusBarHeight);
  const float tabWanted = compact ? static_cast<float>(theme::kTabRailHeight * 2) : static_cast<float>(theme::kTabRailHeight);
  const float tabHeight = (afterHeader - statusHeight) < tabWanted ? ClampNonNegative(afterHeader - statusHeight) : tabWanted;
  const float contentHeight = ClampNonNegative(safeHeight - headerHeight - tabHeight - statusHeight);
  const float padding = safeWidth >= theme::kPrototypeMinimumWidth ? theme::kSpacing[4] : theme::kSpacing[2];
  return {{0.f, 0.f, safeWidth, safeHeight},
          {0.f, 0.f, safeWidth, headerHeight},
          {0.f, headerHeight, safeWidth, tabHeight},
          {0.f, headerHeight + tabHeight, safeWidth, contentHeight},
          {0.f, headerHeight + tabHeight + contentHeight, safeWidth, statusHeight},
          {padding, headerHeight + tabHeight + padding, ClampNonNegative(safeWidth - padding * 2.f), ClampNonNegative(contentHeight - padding * 2.f)},
          safeWidth >= 1150.f ? WidthClass::Wide : (compact ? WidthClass::Compact : WidthClass::Intermediate), compact};
}
}
