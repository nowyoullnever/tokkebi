#pragma once

#include "NavigationState.h"

namespace tokkebi::ui
{
enum class LocalAudioFocusTransition { None, EnterButton, ExitToInbox, ExitToLibrary };

// The local-audio button is the only non-tab shell focus target. It may exist
// only on INBOX and is released before another shell target takes ownership.
class LocalAudioFocusRouter final
{
public:
  bool ButtonFocused() const { return mButtonFocused; }
  bool ConsumesArrowKeys() const { return mButtonFocused; }

  LocalAudioFocusTransition TabTransition(const NavigationState& navigation, bool reverse) const
  {
    if (mButtonFocused)
      return reverse ? LocalAudioFocusTransition::ExitToInbox : LocalAudioFocusTransition::ExitToLibrary;
    if (navigation.Focus() != FocusTarget::TabRail)
      return LocalAudioFocusTransition::None;
    if ((!reverse && navigation.Focused() == TabId::Inbox) || (reverse && navigation.Focused() == TabId::Library))
      return LocalAudioFocusTransition::EnterButton;
    return LocalAudioFocusTransition::None;
  }

  bool EnterButton(NavigationState& navigation, bool reverse)
  {
    if (TabTransition(navigation, reverse) != LocalAudioFocusTransition::EnterButton)
      return false;
    navigation.Select(TabId::Inbox);
    mButtonFocused = true;
    return true;
  }

  bool ExitButton(NavigationState& navigation, bool reverse)
  {
    const auto transition = TabTransition(navigation, reverse);
    if (transition != LocalAudioFocusTransition::ExitToInbox && transition != LocalAudioFocusTransition::ExitToLibrary)
      return false;
    Release();
    navigation.Select(TabId::Inbox);
    if (transition == LocalAudioFocusTransition::ExitToLibrary)
      navigation.MoveFocus(1);
    return true;
  }

  void FocusButton(NavigationState& navigation) { navigation.Select(TabId::Inbox); mButtonFocused = true; }
  void SelectShellTab(NavigationState& navigation, TabId tab) { Release(); navigation.Select(tab); }
  void FocusTheme(NavigationState& navigation) { Release(); navigation.FocusTheme(); }
  void Release() { mButtonFocused = false; }
  void EnforcePageContext(const NavigationState& navigation) { if (navigation.Selected() != TabId::Inbox) Release(); }

private:
  bool mButtonFocused = false;
};
}
