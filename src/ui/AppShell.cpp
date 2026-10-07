#include "AppShell.h"

#include "../app/fonts.h"
#include "text/Strings.h"

namespace tokkebi::ui
{
namespace
{
using namespace iplug::igraphics;

IColor ToIColor(theme::Color color) { return {color.a, color.r, color.g, color.b}; }
IRECT ToIRECT(const IRECT& origin, Rect rect) { return {origin.L + rect.x, origin.T + rect.y, origin.L + rect.x + rect.width, origin.T + rect.y + rect.height}; }
}

AppShell::AppShell(const IRECT& bounds)
: IControl(bounds)
{
}

ShellLayout AppShell::Layout() const { return CalculateShellLayout(mRECT.W(), mRECT.H()); }

Rect AppShell::TabBounds(const ShellLayout& layout, std::size_t index) const
{
  const std::size_t columns = layout.compactTabs ? 3 : kTabs.size();
  const std::size_t row = layout.compactTabs ? index / columns : 0;
  const std::size_t column = layout.compactTabs ? index % columns : index;
  const std::size_t rows = layout.compactTabs ? 2 : 1;
  const float width = columns == 0 ? 0.f : layout.tabRail.width / static_cast<float>(columns);
  const float height = rows == 0 ? 0.f : layout.tabRail.height / static_cast<float>(rows);
  return {layout.tabRail.x + width * static_cast<float>(column), layout.tabRail.y + height * static_cast<float>(row), width, height};
}

Rect AppShell::ThemeBounds(const ShellLayout& layout) const
{
  const float width = layout.header.width < 132.f ? layout.header.width : 132.f;
  const float inset = layout.header.height < 16.f ? layout.header.height / 2.f : 8.f;
  return {layout.header.x + layout.header.width - width + inset, layout.header.y + inset,
          ClampNonNegative(width - inset * 2.f), ClampNonNegative(layout.header.height - inset * 2.f)};
}

TabId AppShell::TabAt(float x, float y, const ShellLayout& layout) const
{
  const float localX = x - mRECT.L;
  const float localY = y - mRECT.T;
  for (std::size_t index = 0; index < kTabs.size(); ++index)
    if (TabBounds(layout, index).Contains(localX, localY))
      return kTabs[index];
  return TabId::Count;
}

void AppShell::Redraw() { SetDirty(false); }

void AppShell::Draw(IGraphics& graphics)
{
  const auto layout = Layout();
  const auto tokens = theme::Get(mState.Theme());
  const auto color = [](theme::Color value) { return ToIColor(value); };
  const auto local = [this](Rect rect) { return ToIRECT(mRECT, rect); };

  graphics.FillRect(color(tokens.app), mRECT);
  graphics.FillRect(color(tokens.secondary), local(layout.header));
  graphics.FillRect(color(tokens.raised), local(layout.tabRail));
  graphics.FillRect(color(tokens.primary), local(layout.content));
  graphics.FillRect(color(tokens.audio), local(layout.statusBar));
  graphics.DrawRect(color(tokens.borderStrong), mRECT, nullptr, theme::kBorderStructural);
  graphics.DrawLine(color(tokens.separator), mRECT.L, mRECT.T + layout.tabRail.y, mRECT.R, mRECT.T + layout.tabRail.y, nullptr, theme::kBorderThin);
  graphics.DrawLine(color(tokens.separator), mRECT.L, mRECT.T + layout.statusBar.y, mRECT.R, mRECT.T + layout.statusBar.y, nullptr, theme::kBorderThin);

  graphics.DrawText({24.f, color(tokens.textPrimary), fonts::kPrimary}, strings::kProductName.data(), local({16.f, 8.f, 180.f, layout.header.height - 16.f}));
  graphics.DrawText({13.f, color(tokens.textPrimary), fonts::kTechnical}, strings::kVersion.data(), local({194.f, 8.f, 80.f, layout.header.height - 16.f}));
  const auto themeBounds = ThemeBounds(layout);
  graphics.FillRect(color(tokens.actionDefault), local(themeBounds));
  graphics.DrawRect(color(tokens.borderFocus), local(themeBounds), nullptr, theme::kBorderThin);
  graphics.DrawText({13.f, color(tokens.actionText), fonts::kPrimary}, mState.Theme() == theme::ThemeMode::Dark ? strings::kThemeDark.data() : strings::kThemeLight.data(), local(themeBounds));

  for (std::size_t index = 0; index < kTabs.size(); ++index)
  {
    const auto tab = kTabs[index];
    const auto bounds = TabBounds(layout, index);
    const bool active = tab == mState.Selected();
    const bool focused = tab == mState.Focused();
    const bool hovered = tab == mHovered;
    if (active)
      graphics.FillRect(color(tokens.actionDefault), local(bounds));
    else if (hovered)
      graphics.FillRect(color(tokens.raised), local(bounds));
    if (focused)
      graphics.DrawRect(color(tokens.focusRing), local(bounds).GetPadded(-2.f), nullptr, theme::kBorderStandard);
    graphics.DrawText({14.f, color(active ? tokens.actionText : tokens.textPrimary), fonts::kPrimary}, strings::TabLabel(tab).data(), local(bounds));
  }

  const auto content = layout.contentPadding;
  graphics.DrawText({24.f, color(tokens.textPrimary), fonts::kPrimary}, strings::TabLabel(mState.Selected()).data(), local({content.x, content.y, content.width, 34.f}));
  graphics.DrawLine(color(tokens.separator), mRECT.L + content.x, mRECT.T + content.y + 42.f, mRECT.L + content.x + content.width, mRECT.T + content.y + 42.f, nullptr, theme::kBorderThin);
  graphics.DrawText({14.f, color(tokens.textPrimary), fonts::kBody}, strings::SectionMessage(mState.Selected()).data(), local({content.x, content.y + 58.f, content.width, content.height > 58.f ? content.height - 58.f : 0.f}));

  const float third = layout.statusBar.width / 3.f;
  graphics.DrawText({12.f, color(tokens.technicalOnAudio), fonts::kTechnical}, strings::kStatusJobs.data(), local({8.f, layout.statusBar.y, third - 8.f, layout.statusBar.height}));
  graphics.DrawText({12.f, color(tokens.technicalOnAudio), fonts::kTechnical}, strings::kStatusHelper.data(), local({third + 8.f, layout.statusBar.y, third - 8.f, layout.statusBar.height}));
  graphics.DrawText({12.f, color(tokens.technicalOnAudio), fonts::kTechnical}, strings::kStatusLibrary.data(), local({third * 2.f + 8.f, layout.statusBar.y, third - 8.f, layout.statusBar.height}));
}

void AppShell::OnMouseDown(float x, float y, const IMouseMod&)
{
  const auto layout = Layout();
  const float localX = x - mRECT.L;
  const float localY = y - mRECT.T;
  if (ThemeBounds(layout).Contains(localX, localY))
    mState.ToggleTheme();
  else
  {
    const auto tab = TabAt(x, y, layout);
    if (IsValid(tab))
      mState.Select(tab);
  }
  Redraw();
}

void AppShell::OnMouseOver(float x, float y, const IMouseMod& mod)
{
  IControl::OnMouseOver(x, y, mod);
  const auto next = TabAt(x, y, Layout());
  if (next != mHovered)
  {
    mHovered = next;
    Redraw();
  }
}

void AppShell::OnMouseOut()
{
  IControl::OnMouseOut();
  if (mHovered != TabId::Count)
  {
    mHovered = TabId::Count;
    Redraw();
  }
}

bool AppShell::OnKeyDown(float, float, const iplug::IKeyPress& key) { return HandleKey(key); }

bool AppShell::HandleKey(const iplug::IKeyPress& key)
{
  if (key.C || key.A)
    return false;
  if (key.VK == iplug::kVK_TAB)
  {
    mState.MoveFocus(key.S ? -1 : 1);
    Redraw();
    return true;
  }
  if (key.VK == iplug::kVK_LEFT || key.VK == iplug::kVK_UP)
  {
    mState.MoveFocus(-1);
    Redraw();
    return true;
  }
  if (key.VK == iplug::kVK_RIGHT || key.VK == iplug::kVK_DOWN)
  {
    mState.MoveFocus(1);
    Redraw();
    return true;
  }
  if (key.VK == iplug::kVK_RETURN || key.VK == iplug::kVK_SPACE)
  {
    mState.ActivateFocused();
    Redraw();
    return true;
  }
  return false;
}
}
