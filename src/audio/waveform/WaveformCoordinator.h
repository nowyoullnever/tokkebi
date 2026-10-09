#pragma once
#include "Waveform.h"
#include <functional>
#include <memory>
namespace tokkebi::audio::waveform { enum class BuildState{Empty,Building,Ready,Failed}; class Coordinator { public: Coordinator();~Coordinator();void Begin(const DecodedPcm&,uint64_t generation);bool Apply(uint64_t generation);BuildState Status()const;std::shared_ptr<const PeakCache> Cache()const;private:struct SharedState;std::shared_ptr<SharedState>m;}; }
