#pragma once

#include <array>
#include <cstddef>

namespace tokkebi::ui
{
enum class TabId { Web, P2P, Inbox, Library, History, Settings, Count };

inline constexpr std::array<TabId, 6> kTabs {{
  TabId::Web, TabId::P2P, TabId::Inbox, TabId::Library, TabId::History, TabId::Settings
}};

constexpr bool IsValid(TabId tab) { return static_cast<std::size_t>(tab) < kTabs.size(); }
constexpr std::size_t TabIndex(TabId tab) { return static_cast<std::size_t>(tab); }
}
