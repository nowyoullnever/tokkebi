#include "WaveformCoordinator.h"
#include <condition_variable>
#include <mutex>
#include <optional>
#include <thread>
namespace tokkebi::audio::waveform {
struct Coordinator::SharedState { std::mutex mutex; std::condition_variable wake; std::optional<std::pair<uint64_t,DecodedPcm>> pending; std::optional<std::pair<uint64_t,std::shared_ptr<const PeakCache>>> done; std::shared_ptr<const PeakCache> cache; BuildState status=BuildState::Empty; bool stopping=false; std::thread worker; };
Coordinator::Coordinator():m(std::make_shared<SharedState>()) { auto s=m; m->worker=std::thread([s]{for(;;){std::pair<uint64_t,DecodedPcm> job;{std::unique_lock lock(s->mutex);s->wake.wait(lock,[&]{return s->stopping||s->pending.has_value();});if(s->stopping)return;job=std::move(*s->pending);s->pending.reset();}auto cache=BuildPeakCache(job.second);std::lock_guard lock(s->mutex);if(!s->stopping)s->done=std::make_pair(job.first,std::move(cache));}}); }
Coordinator::~Coordinator(){{std::lock_guard lock(m->mutex);m->stopping=true;m->pending.reset();}m->wake.notify_one();m->worker.join();}
void Coordinator::Begin(const DecodedPcm& pcm,uint64_t generation){std::lock_guard lock(m->mutex);m->cache.reset();m->status=BuildState::Building;m->pending=std::make_pair(generation,pcm);m->wake.notify_one();}
bool Coordinator::Apply(uint64_t generation){std::lock_guard lock(m->mutex);if(!m->done||m->done->first!=generation)return false;m->cache=m->done->second;m->status=m->cache?BuildState::Ready:BuildState::Failed;m->done.reset();return true;}
BuildState Coordinator::Status()const{std::lock_guard lock(m->mutex);return m->status;}std::shared_ptr<const PeakCache> Coordinator::Cache()const{std::lock_guard lock(m->mutex);return m->cache;}
}
