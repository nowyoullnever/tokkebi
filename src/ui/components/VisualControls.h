#pragma once

#include "IControls.h"

#include "../../app/fonts.h"
#include "../layout/ShellLayout.h"
#include "../theme/Theme.h"
#include "Components.h"

namespace tokkebi::ui::components
{
using namespace iplug::igraphics;
inline IColor ControlColor(theme::Color value) { return {value.a, value.r, value.g, value.b}; }
inline IRECT ControlRect(const IRECT& origin, Rect value) { return {origin.L + value.x, origin.T + value.y, origin.L + value.x + value.width, origin.T + value.y + value.height}; }

class ButtonControl
{
public:
  ButtonControl(std::string label, std::function<void()> callback = {}) : mLabel(std::move(label)), mModel(std::move(callback)) {}
  void SetBounds(Rect bounds) { mBounds = bounds; }
  void SetEnabled(bool enabled) { mModel.SetState(enabled ? ButtonState::Normal : ButtonState::Disabled); }
  bool Enabled() const { return mModel.State() != ButtonState::Disabled && mModel.State() != ButtonState::Busy; }
  bool Hit(float x, float y) const { return mBounds.Contains(x, y); }
  bool OnMouseDown(float x, float y) { return Hit(x, y) && mModel.Activate(); }
  bool OnKey(const iplug::IKeyPress& key) { return key.VK == iplug::kVK_RETURN || key.VK == iplug::kVK_SPACE ? mModel.Activate() : false; }
  ButtonModel& Model() { return mModel; }
  const ButtonModel& Model() const { return mModel; }
  void Draw(IGraphics& graphics, const IRECT& origin, const theme::Tokens& tokens, bool focused) const
  {
    const bool enabled = Enabled(); const auto bounds = ControlRect(origin, mBounds);
    graphics.FillRect(ControlColor(enabled ? tokens.actionDefault : tokens.actionDisabled), bounds);
    graphics.DrawRect(ControlColor(focused ? (enabled ? tokens.focusRing : tokens.borderStrong) : tokens.borderDefault), bounds, nullptr, focused ? theme::kBorderStandard : theme::kBorderThin);
    graphics.DrawText({12.f, ControlColor(enabled ? tokens.actionText : tokens.textDisabled), fonts::kPrimary}, mLabel.c_str(), bounds);
  }
private: Rect mBounds {}; std::string mLabel; ButtonModel mModel;
};

class TextFieldControl
{
public:
  explicit TextFieldControl(std::size_t maximum) : mModel(maximum) {}
  void SetBounds(Rect bounds) { mBounds = bounds; }
  bool Hit(float x, float y) const { return mBounds.Contains(x, y); }
  TextFieldModel& Model() { return mModel; }
  const TextFieldModel& Model() const { return mModel; }
  void Draw(IGraphics& graphics, const IRECT& origin, const theme::Tokens& tokens, bool focused) const
  {
    const auto bounds = ControlRect(origin, mBounds); const bool empty = mModel.Text().empty();
    graphics.FillRect(ControlColor(tokens.recessed), bounds);
    graphics.DrawRect(ControlColor(focused ? tokens.focusRing : tokens.borderDefault), bounds, nullptr, focused ? theme::kBorderStandard : theme::kBorderThin);
    const std::string shown = empty ? "Type temporary text (not saved)" : mModel.Text();
    graphics.DrawText({13.f, ControlColor(empty ? tokens.textMuted : tokens.textPrimary), fonts::kPrimary}, shown.c_str(), bounds.GetHPadded(-6.f));
  }
private: Rect mBounds {}; TextFieldModel mModel;
};

class ListViewControl
{
public:
  void SetBounds(Rect bounds) { mBounds = bounds; }
  bool Hit(float x, float y) const { return mBounds.Contains(x, y); }
  ListModel& Model() { return mModel; }
  const ListModel& Model() const { return mModel; }
  bool OnMouseDown(float x, float y)
  {
    if (!Hit(x, y)) return false; const auto row = mModel.VisibleRange().first + static_cast<std::size_t>((y - mBounds.y - 2.f) / 22.f);
    return row < mModel.Rows().size() ? mModel.Select(mModel.Rows()[row].id) : false;
  }
  void OnWheel(float distance) { mModel.Scroll(distance > 0.f ? -1 : 1); }
  bool OnKey(const iplug::IKeyPress& key) { return key.VK == iplug::kVK_UP ? mModel.MoveSelection(-1) : (key.VK == iplug::kVK_DOWN ? mModel.MoveSelection(1) : false); }
  void Draw(IGraphics& graphics, const IRECT& origin, const theme::Tokens& tokens, bool focused) const
  {
    const auto bounds = ControlRect(origin, mBounds); graphics.DrawRect(ControlColor(focused ? tokens.focusRing : tokens.borderDefault), bounds, nullptr, focused ? theme::kBorderStandard : theme::kBorderThin);
    const auto range = mModel.VisibleRange();
    for (std::size_t row = range.first; row < range.second; ++row)
    {
      const auto rowBounds = IRECT(bounds.L + 2.f, bounds.T + static_cast<float>(row - range.first) * 22.f + 2.f, bounds.R - 2.f, bounds.T + static_cast<float>(row - range.first) * 22.f + 22.f);
      const bool selected = std::find(mModel.Selected().begin(), mModel.Selected().end(), mModel.Rows()[row].id) != mModel.Selected().end();
      if (selected) graphics.FillRect(ControlColor(tokens.selectionFill), rowBounds);
      graphics.DrawText({12.f, ControlColor(mModel.Rows()[row].disabled ? tokens.textDisabled : (selected ? tokens.selectionText : tokens.textPrimary)), fonts::kPrimary}, mModel.Rows()[row].label.c_str(), rowBounds.GetHPadded(-4.f));
    }
  }
private: Rect mBounds {}; ListModel mModel;
};

class ProgressControl
{
public:
  void SetBounds(Rect bounds) { mBounds = bounds; } ProgressModel& Model() { return mModel; }
  void Draw(IGraphics& graphics, const IRECT& origin, const theme::Tokens& tokens) const
  {
    const auto bounds = ControlRect(origin, mBounds); graphics.FillRect(ControlColor(tokens.recessed), bounds);
    if (mModel.State() == ProgressState::Determinate) graphics.FillRect(ControlColor(tokens.busy), IRECT(bounds.L, bounds.T, bounds.L + bounds.W() * static_cast<float>(mModel.Fraction()), bounds.B));
  }
private: Rect mBounds {}; ProgressModel mModel;
};

class NotificationControl
{
public:
  void SetBounds(Rect bounds) { mBounds = bounds; } NotificationModel& Model() { return mModel; } const NotificationModel& Model() const { return mModel; }
  bool Hit(float x, float y) const { return mModel.Current().has_value() && mBounds.Contains(x, y); }
  bool OnMouseDown(float x, float y) { if (!Hit(x, y)) return false; mModel.Dismiss(); return true; }
  void Draw(IGraphics& graphics, const IRECT& origin, const theme::Tokens& tokens) const
  {
    if (!mModel.Current()) return; const auto bounds = ControlRect(origin, mBounds); graphics.FillRect(ControlColor(tokens.success), bounds);
    const std::string label = "SUCCESS: " + mModel.Current()->message + " [DISMISS]"; graphics.DrawText({11.f, ControlColor(tokens.textInverse), fonts::kPrimary}, label.c_str(), bounds.GetHPadded(-4.f));
  }
private: Rect mBounds {}; NotificationModel mModel;
};
}
