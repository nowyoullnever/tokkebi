#pragma once

#include "config.h"
#include "IPlug_include_in_plug_hdr.h"

using namespace iplug;

class SampleGrabber final : public Plugin
{
public:
  explicit SampleGrabber(const InstanceInfo& info);

#if IPLUG_DSP
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
