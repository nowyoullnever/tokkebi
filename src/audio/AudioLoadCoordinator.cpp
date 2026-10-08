#include "AudioLoadCoordinator.h"
#include "AudioDocument.h"

#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace tokkebi::audio {
struct AudioLoadCoordinator::SharedState {
  std::mutex mutex;
  std::vector<std::pair<uint64_t, AudioLoadResult>> completed;
  std::vector<std::thread> workers;
  bool accepting = true;
};
AudioLoadCoordinator::AudioLoadCoordinator(DecodeFunction decode) : mShared(std::make_shared<SharedState>()), mDecode(std::move(decode)) {}
AudioLoadCoordinator::~AudioLoadCoordinator() {
  const auto shared = mShared;
  { std::lock_guard lock(shared->mutex); shared->accepting = false; }
  for (auto& worker : shared->workers) if (worker.joinable()) worker.join();
}
void AudioLoadCoordinator::Begin(const std::filesystem::path& path, AudioDocument& document) {
  document.BeginLoad(); const uint64_t generation = ++mGeneration; const auto shared = mShared; const auto decode = mDecode;
  std::thread worker([shared, decode, generation, path] {
    AudioLoadResult result = decode(path);
    std::lock_guard lock(shared->mutex);
    if (shared->accepting) shared->completed.emplace_back(generation, std::move(result));
  });
  std::lock_guard lock(shared->mutex); shared->workers.emplace_back(std::move(worker));
}
bool AudioLoadCoordinator::ApplyCompleted(AudioDocument& document) {
  std::vector<std::pair<uint64_t, AudioLoadResult>> completed;
  { std::lock_guard lock(mShared->mutex); completed.swap(mShared->completed); }
  bool applied = false;
  for (auto& item : completed) if (item.first == mGeneration) { document.Complete(std::move(item.second)); applied = true; }
  return applied;
}
}
