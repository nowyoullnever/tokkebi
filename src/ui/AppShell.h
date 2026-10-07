#pragma once

#include "IControls.h"

#include "layout/ShellLayout.h"
#include "navigation/NavigationState.h"

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
  bool OnKeyDown(float x, float y, const iplug::IKeyPress& key) override;

  bool HandleKey(const iplug::IKeyPress& key);
  const NavigationState& State() const { return mState; }

private:
  ShellLayout Layout() const;
  Rect TabBounds(const ShellLayout& layout, std::size_t index) const;
  Rect ThemeBounds(const ShellLayout& layout) const;
  TabId TabAt(float x, float y, const ShellLayout& layout) const;
  void Redraw();

  NavigationState mState;
  TabId mHovered = TabId::Count;
};
}
