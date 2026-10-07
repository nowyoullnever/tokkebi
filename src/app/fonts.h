#pragma once

#include "IGraphics.h"

namespace tokkebi::fonts
{
inline constexpr char kPrimary[] = "tokkebi-primary";
inline constexpr char kBody[] = "tokkebi-body";
inline constexpr char kTechnical[] = "tokkebi-technical";

#if defined(_WIN32)
inline constexpr char kPrimaryResource[] = "TOKKEBI_PRIMARY_FONT";
inline constexpr char kBodyResource[] = "TOKKEBI_BODY_FONT";
inline constexpr char kTechnicalResource[] = "TOKKEBI_TECHNICAL_FONT";
#else
inline constexpr char kPrimaryResource[] = "dunggeunmo.ttf";
inline constexpr char kBodyResource[] = "ridibatang.otf";
inline constexpr char kTechnicalResource[] = "galmuri11-bitmap-regular-2.40.4.ttf";
#endif

inline bool Load(iplug::igraphics::IGraphics* graphics)
{
  return graphics->LoadFont(kPrimary, kPrimaryResource)
      && graphics->LoadFont(kBody, kBodyResource)
      && graphics->LoadFont(kTechnical, kTechnicalResource);
}
}
