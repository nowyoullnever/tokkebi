#pragma once

#include "AudioDecoder.h"

#include <functional>
#include <memory>
#include <thread>

namespace tokkebi::audio {
class AudioDocument;
class AudioLoadCoordinator final {
public:
  using DecodeFunction = std::function<AudioLoadResult(const std::filesystem::path&)>;
  explicit AudioLoadCoordinator(DecodeFunction decode = DecodeLocalAudio);
  ~AudioLoadCoordinator();
  AudioLoadCoordinator(const AudioLoadCoordinator&) = delete;
  AudioLoadCoordinator& operator=(const AudioLoadCoordinator&) = delete;
  void Begin(const std::filesystem::path& path, AudioDocument& document);
  bool ApplyCompleted(AudioDocument& document);
private:
  struct SharedState;
  std::shared_ptr<SharedState> mShared;
  DecodeFunction mDecode;
  uint64_t mGeneration = 0;
  std::thread mWorker;
};
}
