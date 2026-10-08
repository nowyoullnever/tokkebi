#pragma once
#include "AudioTypes.h"
#include <filesystem>
#include <string>
namespace tokkebi::audio {
struct AudioFileInfo { std::filesystem::path path; AudioContainer container{AudioContainer::Unknown}; AudioCodec codec{AudioCodec::Unknown}; SampleFormat sourceFormat{SampleFormat::Unknown}; uint32_t channels{}; uint32_t sampleRate{}; uint64_t frames{}; double durationSeconds{}; uint64_t fileSize{}; };
struct AudioLoadResult { AudioFileInfo info; DecodedPcm audio; DecodeErrorCode error{DecodeErrorCode::None}; std::string diagnostic; bool Success() const { return error == DecodeErrorCode::None; } };
struct DecodedAllocation { uint64_t sampleCount{}; uint64_t bytes{}; };
inline constexpr uint64_t kMaximumDecodedBytes = 512ULL * 1024ULL * 1024ULL;
bool CalculateDecodedAllocation(uint64_t frames, uint32_t channels, DecodedAllocation& allocation);
AudioLoadResult DecodeLocalAudio(const std::filesystem::path& path);
double FramesToSeconds(uint64_t frames, uint32_t sampleRate);
uint64_t SecondsToFrames(double seconds, uint32_t sampleRate);
double FramesToMilliseconds(uint64_t frames, uint32_t sampleRate);
uint64_t MillisecondsToFrames(double milliseconds, uint32_t sampleRate);
}
