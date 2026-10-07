#pragma once

#include "IControls.h"

#include "layout/ShellLayout.h"
#include "navigation/NavigationState.h"
#include "components/Components.h"

namespace tokkebi::ui
{
class AppShell final : public iplug::igraphics::IControl
{
public:
  explicit AppShell(const iplug::igraphics::IRECT& bounds);

  void Draw(iplug::igraphics::IGraphics& graphics) override;
  void OnMouseDown(float x, float y, const iplug::igraphics::IMouseMod& mod) override;
  void OnMouseOver(float x, float y, const iplug::igraphics::IMouseMod& mod) override;
  void OnMouseOut() override;
  void OnMouseWheel(float x, float y, const iplug::igraphics::IMouseMod& mod, float distance) override;
  bool OnKeyDown(float x, float y, const iplug::IKeyPress& key) override;

  bool HandleKey(const iplug::IKeyPress& key);
  const NavigationState& State() const { return mState; }

private:
  ShellLayout Layout() const;
  Rect TabBounds(const ShellLayout& layout, std::size_t index) const;
  Rect ThemeBounds(const ShellLayout& layout) const;
  TabId TabAt(float x, float y, const ShellLayout& layout) const;
  void DrawComponentDemo(iplug::igraphics::IGraphics& graphics, const ShellLayout& layout);
  bool HandleDemoKey(const iplug::IKeyPress& key);
  void Redraw();

  NavigationState mState;
  components::ButtonModel mDemoButton;
  components::TextFieldModel mDemoText {64};
  components::ListModel mDemoList;
  components::ModalModel mDemoModal;
  components::NotificationModel mDemoNotification;
  components::ProgressModel mDemoProgress;
  components::FocusRouter mDemoFocus;
  bool mDemoListFocused = false;
};
}
