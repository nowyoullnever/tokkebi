#include "SampleGrabber.h"

#include "IControls.h"
#include "IPlug_include_in_plug_src.h"

namespace
{
const iplug::igraphics::IColor kBackground {255, 0x40, 0x30, 0x20};
const iplug::igraphics::IColor kBorder {255, 0xB0, 0x60, 0x70};
const iplug::igraphics::IColor kAccent {255, 0x50, 0xA0, 0xB0};
const iplug::igraphics::IColor kSecondary {255, 0x78, 0x48, 0x60};
}

SampleGrabber::SampleGrabber(const iplug::InstanceInfo& info)
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
    graphics->AttachControl(new ILambdaControl(graphics->GetBounds(),
      [](ILambdaControl*, IGraphics& g, IRECT& bounds) {
        const IRECT frame = bounds.GetPadded(-36.f);
        g.DrawRect(kBorder, frame, nullptr, 3.f);
        g.FillRect(kSecondary, frame.GetFromTop(4.f));
        g.FillRect(kAccent, frame.GetFromBottom(4.f));
      }));
  };
#endif
}

#if IPLUG_DSP
void SampleGrabber::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
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
