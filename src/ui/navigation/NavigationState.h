#pragma once

#include "TabId.h"
#include "../theme/Theme.h"

namespace tokkebi::ui
{
enum class FocusTarget { TabRail, ThemeButton };

class NavigationState
{
public:
  TabId Selected() const { return mSelected; }
  TabId Focused() const { return mFocused; }
  TabId Hovered() const { return mHovered; }
  FocusTarget Focus() const { return mFocus; }
  bool ThemeFocused() const { return mFocus == FocusTarget::ThemeButton; }
  theme::ThemeMode Theme() const { return mTheme; }

  bool Select(TabId tab)
  {
    if (!IsValid(tab))
      return false;
    const bool changed = mSelected != tab;
    mSelected = tab;
    mFocused = tab;
    mFocus = FocusTarget::TabRail;
    return changed;
  }

  bool SetHovered(TabId tab)
  {
    const TabId next = IsValid(tab) ? tab : TabId::Count;
    if (mHovered == next)
      return false;
    mHovered = next;
    return true;
  }

  void MoveFocus(int direction)
  {
    if (direction < 0)
    {
      if (mFocus == FocusTarget::ThemeButton)
      {
        mFocus = FocusTarget::TabRail;
        mFocused = TabId::Settings;
      }
      else if (mFocused == TabId::Web)
        mFocus = FocusTarget::ThemeButton;
      else
        mFocused = kTabs[TabIndex(mFocused) - 1];
      return;
    }

    if (mFocus == FocusTarget::ThemeButton)
    {
      mFocus = FocusTarget::TabRail;
      mFocused = TabId::Web;
    }
    else if (mFocused == TabId::Settings)
      mFocus = FocusTarget::ThemeButton;
    else
      mFocused = kTabs[TabIndex(mFocused) + 1];
  }

  void MoveTabFocus(int direction)
  {
    if (mFocus != FocusTarget::TabRail)
      return;
    const int count = static_cast<int>(kTabs.size());
    int index = static_cast<int>(TabIndex(mFocused)) + (direction < 0 ? -1 : 1);
    if (index < 0)
      index += count;
    if (index >= count)
      index -= count;
    mFocused = kTabs[static_cast<std::size_t>(index)];
  }

  bool ActivateFocused()
  {
    if (mFocus == FocusTarget::ThemeButton)
    {
      ToggleTheme();
      return true;
    }
    return Select(mFocused);
  }

  void FocusTheme() { mFocus = FocusTarget::ThemeButton; }
  void ToggleTheme() { mTheme = mTheme == theme::ThemeMode::Dark ? theme::ThemeMode::Light : theme::ThemeMode::Dark; }

private:
  TabId mSelected = TabId::Web;
  TabId mFocused = TabId::Web;
  TabId mHovered = TabId::Count;
  FocusTarget mFocus = FocusTarget::TabRail;
  theme::ThemeMode mTheme = theme::ThemeMode::Dark;
};
}
