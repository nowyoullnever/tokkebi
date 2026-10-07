#pragma once

#include "TabId.h"
#include "../theme/Theme.h"

namespace tokkebi::ui
{
class NavigationState
{
public:
  TabId Selected() const { return mSelected; }
  TabId Focused() const { return mFocused; }
  theme::ThemeMode Theme() const { return mTheme; }

  bool Select(TabId tab)
  {
    if (!IsValid(tab))
      return false;
    const bool changed = mSelected != tab;
    mSelected = tab;
    mFocused = tab;
    return changed;
  }

  void MoveFocus(int direction)
  {
    const int count = static_cast<int>(kTabs.size());
    int index = static_cast<int>(TabIndex(mFocused)) + (direction < 0 ? -1 : 1);
    if (index < 0)
      index += count;
    if (index >= count)
      index -= count;
    mFocused = kTabs[static_cast<std::size_t>(index)];
  }

  bool ActivateFocused() { return Select(mFocused); }
  void ToggleTheme() { mTheme = mTheme == theme::ThemeMode::Dark ? theme::ThemeMode::Light : theme::ThemeMode::Dark; }

private:
  TabId mSelected = TabId::Web;
  TabId mFocused = TabId::Web;
  theme::ThemeMode mTheme = theme::ThemeMode::Dark;
};
}
