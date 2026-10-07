#include "tokkebi.h"
#include "fonts.h"
#include "../ui/theme/Theme.h"

#include "IControls.h"
#include "IPlug_include_in_plug_src.h"

#include <cstdio>

namespace
{
class ThemePreviewControl final : public iplug::igraphics::IControl
{
public:
  ThemePreviewControl(const iplug::igraphics::IRECT& bounds) : IControl(bounds) {}
  void Draw(iplug::igraphics::IGraphics& g) override
  {
    using namespace iplug::igraphics;
    const auto tokens = tokkebi::theme::Get(mMode);
    const auto color = [](tokkebi::theme::Color c) { return IColor {c.a, c.r, c.g, c.b}; };
    g.FillRect(color(tokens.app), mRECT);
    const IRECT frame = mRECT.GetPadded(-36.f);
    g.FillRect(color(tokens.secondary), frame.GetFromTop(52.f));
    g.FillRect(color(tokens.audio), frame.GetFromBottom(74.f));
    g.DrawRect(color(tokens.borderStrong), frame, nullptr, tokkebi::theme::kBorderStructural);
    g.FillRect(color(tokens.selectionFill), IRECT(frame.L + 24, frame.T + 78, frame.L + 250, frame.T + 114));
    g.DrawRect(color(tokens.selectionBorder), IRECT(frame.L + 24, frame.T + 78, frame.L + 250, frame.T + 114), nullptr, tokkebi::theme::kBorderStandard);
    g.DrawText({28.f, color(tokens.textPrimary), tokkebi::fonts::kPrimary}, "tokkebi", frame.GetFromTop(46.f));
    g.DrawText({14.f, color(tokens.textPrimary), tokkebi::fonts::kPrimary}, mMode == tokkebi::theme::ThemeMode::Dark ? "DARK PREVIEW - CLICK TO SWITCH" : "LIGHT PREVIEW - CLICK TO SWITCH", IRECT(frame.L + 270, frame.T, frame.R - 16, frame.T + 52));
    g.DrawText({20.f, color(tokens.textPrimary), tokkebi::fonts::kPrimary}, "도깨비", IRECT(frame.L + 24, frame.T + 126, frame.R, frame.T + 162));
    g.DrawText({14.f, color(tokens.textPrimary), tokkebi::fonts::kBody}, "오디오를 불러와 필요한 구간을 선택합니다.", IRECT(frame.L + 24, frame.T + 178, frame.R - 24, frame.T + 210));
    g.DrawText({14.f, color(tokens.textSecondary), tokkebi::fonts::kBody}, "Sample description and instructions.", IRECT(frame.L + 24, frame.T + 214, frame.R - 24, frame.T + 246));
    g.DrawText({13.f, color(tokens.technicalOnAudio), tokkebi::fonts::kTechnical}, "00:01:23.456", IRECT(frame.L + 24, frame.B - 66, frame.R, frame.B - 42));
    g.DrawText({13.f, color(tokens.technicalOnAudio), tokkebi::fonts::kTechnical}, "44.1 kHz / 24-bit / Stereo", IRECT(frame.L + 24, frame.B - 38, frame.R, frame.B - 14));
  }
  void OnMouseDown(float, float, const iplug::igraphics::IMouseMod&) override { mMode = mMode == tokkebi::theme::ThemeMode::Dark ? tokkebi::theme::ThemeMode::Light : tokkebi::theme::ThemeMode::Dark; SetDirty(false); }
private:
  tokkebi::theme::ThemeMode mMode = tokkebi::theme::ThemeMode::Dark;
};
}

Tokkebi::Tokkebi(const iplug::InstanceInfo& info)
: Plugin(info, MakeConfig(0, 0))
{
#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS,
                        GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };

  mLayoutFunc = [](iplug::igraphics::IGraphics* graphics) {
    using namespace iplug::igraphics;

    const bool fontsLoaded = tokkebi::fonts::Load(graphics);
    if (!fontsLoaded)
    {
      std::fputs("tokkebi: required bundled fonts could not be loaded\n", stderr);
      return;
    }
    graphics->AttachControl(new ThemePreviewControl(graphics->GetBounds()));
  };
#endif
}

#if IPLUG_DSP
void Tokkebi::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
#if defined(APP_API)
  // P00.2 Standalone intentionally has no input bus and must remain silent.
  static_cast<void>(inputs);
  const int outputChannels = NOutChansConnected();
  for (int channel = 0; channel < outputChannels; ++channel)
  {
    for (int frame = 0; frame < nFrames; ++frame)
      outputs[channel][frame] = 0.0;
  }
#else
  const int inputChannels = NInChansConnected();
  const int outputChannels = NOutChansConnected();

  for (int channel = 0; channel < outputChannels; ++channel)
  {
    for (int frame = 0; frame < nFrames; ++frame)
      outputs[channel][frame] = channel < inputChannels ? inputs[channel][frame] : 0.0;
  }
#endif
}
#endif
