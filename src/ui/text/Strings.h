#pragma once

#include "../navigation/TabId.h"

#include <string_view>

namespace tokkebi::ui::strings
{
inline constexpr std::string_view kProductName = "tokkebi";
inline constexpr std::string_view kVersion = "v0.1.0";
inline constexpr std::string_view kThemeDark = "THEME: DARK";
inline constexpr std::string_view kThemeLight = "THEME: LIGHT";
inline constexpr std::string_view kStatusJobs = "JOBS: 아직 활성 작업 시스템이 없습니다";
inline constexpr std::string_view kStatusHelper = "HELPER: 초기화되지 않음";
inline constexpr std::string_view kStatusLibrary = "LIBRARY: 초기화되지 않음";

constexpr std::string_view TabLabel(TabId tab)
{
  constexpr std::string_view labels[] = {"WEB", "P2P", "INBOX", "LIBRARY", "HISTORY", "SETTINGS"};
  return IsValid(tab) ? labels[TabIndex(tab)] : "";
}

constexpr std::string_view SectionMessage(TabId tab)
{
  constexpr std::string_view messages[] = {
    "URL 및 소스 기반 수집은 다음 단계에서 구현됩니다.",
    "외부 P2P 연동은 아직 구성되지 않았습니다.",
    "Inbox 시스템은 아직 초기화되지 않았습니다.",
    "라이브러리 저장소는 아직 구현되지 않았습니다.",
    "History 저장소는 아직 초기화되지 않았습니다. 미래 기록은 source, tags, description만 포함합니다.",
    "테마 전환은 현재 이 세션에서만 적용됩니다. 설정은 저장되지 않습니다."
  };
  return IsValid(tab) ? messages[TabIndex(tab)] : "유효하지 않은 섹션입니다.";
}
}
