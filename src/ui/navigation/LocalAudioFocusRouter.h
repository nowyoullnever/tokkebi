#pragma once

#include "NavigationState.h"

namespace tokkebi::ui
{
enum class LocalAudioFocusTransition { None, EnterButton, ExitToInbox, ExitToLibrary };

// The local-audio button is a single focusable item between INBOX and LIBRARY.
class LocalAudioFocusRouter final
{
public:
  static LocalAudioFocusTransition TabTransition(bool buttonFocused, const NavigationState& navigation, bool reverse)
  {
    if (buttonFocused)
      return reverse ? LocalAudioFocusTransition::ExitToInbox : LocalAudioFocusTransition::ExitToLibrary;
    if (navigation.Focus() != FocusTarget::TabRail)
      return LocalAudioFocusTransition::None;
    if ((!reverse && navigation.Focused() == TabId::Inbox) || (reverse && navigation.Focused() == TabId::Library))
      return LocalAudioFocusTransition::EnterButton;
    return LocalAudioFocusTransition::None;
  }
};
}
