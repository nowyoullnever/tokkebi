#include "tokkebi.h"
#include "fonts.h"

#include "IControls.h"
#include "IPlug_include_in_plug_src.h"

#include <cstdio>

namespace
{
const iplug::igraphics::IColor kBackground {255, 0x40, 0x30, 0x20};
const iplug::igraphics::IColor kBorder {255, 0xB0, 0x60, 0x70};
const iplug::igraphics::IColor kAccent {255, 0x50, 0xA0, 0xB0};
const iplug::igraphics::IColor kSecondary {255, 0x78, 0x48, 0x60};
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

    graphics->AttachPanelBackground(kBackground);
    const bool fontsLoaded = tokkebi::fonts::Load(graphics);
    graphics->AttachControl(new ILambdaControl(graphics->GetBounds(),
      [](ILambdaControl*, IGraphics& g, IRECT& bounds) {
        const IRECT frame = bounds.GetPadded(-36.f);
        g.DrawRect(kBorder, frame, nullptr, 3.f);
        g.FillRect(kSecondary, frame.GetFromTop(4.f));
        g.FillRect(kAccent, frame.GetFromBottom(4.f));
      }));
    if (!fontsLoaded)
    {
      std::fputs("tokkebi: required bundled fonts could not be loaded\n", stderr);
      return;
    }
    const IRECT textArea = graphics->GetBounds().GetPadded(-72.f);
    graphics->AttachControl(new ITextControl(textArea.GetFromTop(54.f), "tokkebi", IText(32.f, kBorder, tokkebi::fonts::kPrimary)));
    graphics->AttachControl(new ITextControl(textArea.GetFromTop(94.f).GetVShifted(62.f), "도깨비", IText(26.f, kBorder, tokkebi::fonts::kPrimary)));
    graphics->AttachControl(new ITextControl(textArea.GetFromTop(130.f).GetVShifted(110.f), "WEB  P2P  INBOX  LIBRARY", IText(16.f, kAccent, tokkebi::fonts::kPrimary)));
    graphics->AttachControl(new ITextControl(textArea.GetFromTop(190.f).GetVShifted(180.f), "오디오를 불러와 필요한 구간을 선택합니다.", IText(20.f, kBorder, tokkebi::fonts::kBody)));
    graphics->AttachControl(new ITextControl(textArea.GetFromTop(226.f).GetVShifted(222.f), "Sample description and instructions.", IText(18.f, kBorder, tokkebi::fonts::kBody)));
    graphics->AttachControl(new ITextControl(textArea.GetFromTop(270.f).GetVShifted(280.f), "00:01:23.456", IText(18.f, kAccent, tokkebi::fonts::kTechnical)));
    graphics->AttachControl(new ITextControl(textArea.GetFromTop(306.f).GetVShifted(320.f), "44.1 kHz / 24-bit / Stereo", IText(16.f, kAccent, tokkebi::fonts::kTechnical)));
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
