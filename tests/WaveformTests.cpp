#include "audio/waveform/Waveform.h"
#include "audio/waveform/WaveformCoordinator.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
using namespace tokkebi::audio; using namespace tokkebi::audio::waveform; int failed=0;
#define CHECK(x) do { if (!(x)) { ++failed; std::cerr << #x << '\n'; } } while (0)
int main() {
  DecodedPcm mono{1,48000,7,{-1,-.5f,0,.25f,.5f,1,-.25f}}; auto cache=BuildPeakCache(mono,3); CHECK(cache&&cache->levels.size()>1); CHECK(cache->levels[0].channels[0].size()==3); CHECK(cache->levels[0].channels[0][2].endFrame==7);
  DecodedPcm stereo{2,48000,4,{.1f,-1,.2f,1,.3f,-.5f,.4f,.5f}}; auto sc=BuildPeakCache(stereo,2); CHECK(sc&&sc->levels[0].channels[0][0].minimum>.0f&&sc->levels[0].channels[1][0].minimum==-1);
  DecodedPcm overflow{2,1,UINT64_MAX,{}}; CHECK(!BuildPeakCache(overflow)); DecodedPcm bad{1,1,1,{NAN}}; CHECK(!BuildPeakCache(bad)); DecodedPcm empty{1,1,0,{}}; CHECK(BuildPeakCache(empty));
  auto view=FullViewport(UINT64_MAX); auto z=Zoom(view,4,.5,10); CHECK(z.end>z.start); CHECK(Scroll(z,INT64_MIN).start==0); CHECK(XToFrame(z,FrameToX(z,z.start+1,800),800)>=z.start);
  DecodedPcm longPcm{1,1,65536,std::vector<float>(65536,.25f)}; auto longCache=BuildPeakCache(longPcm,1); auto plan=BuildRenderPlan(*longCache,{65536,32000,32100},100); CHECK(plan.columns.size()<=102); CHECK(plan.inspectedBuckets<=102); CHECK(!BuildTimeRuler({48000,0,48000},48000,400).empty()); CHECK(FormatTime(UINT64_MAX,48000).size()>8);
  auto snapshot=std::make_shared<DecodedPcm>(mono); Coordinator coordinator([](const DecodedPcm&,std::string*)->std::shared_ptr<const PeakCache>{throw std::bad_alloc();}); coordinator.Begin(snapshot,1); for(int i=0;i<100&&!coordinator.Apply(1);++i)std::this_thread::sleep_for(std::chrono::milliseconds(2)); CHECK(coordinator.Status()==BuildState::Failed); CHECK(!coordinator.Diagnostic().empty());
  Coordinator stale; stale.Begin(snapshot,1); stale.Invalidate(2); for(int i=0;i<100&&!stale.Apply(2);++i)std::this_thread::sleep_for(std::chrono::milliseconds(1)); CHECK(!stale.Cache()); stale.Begin(snapshot,2); for(int i=0;i<100&&!stale.Apply(2);++i)std::this_thread::sleep_for(std::chrono::milliseconds(2)); CHECK(stale.Status()==BuildState::Ready&&stale.Cache());
  return failed;
}
