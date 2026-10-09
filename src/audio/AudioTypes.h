#pragma once
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>
namespace tokkebi::audio {
enum class AudioContainer { Unknown, Wav, Aiff, Flac };
enum class AudioCodec { Unknown, Pcm, IeeeFloat };
enum class SampleFormat { Unknown, Int16, Int24, Int32, Float32 };
enum class DecodeErrorCode { None, FileNotFound, PermissionDenied, NotRegularFile, UnsupportedFormat, CorruptFile, TruncatedFile, InvalidMetadata, DecodeFailed, TooLarge, IoError };
struct DecodedPcm {
  uint32_t channels{}; uint32_t sampleRate{}; uint64_t frames{}; std::vector<float> interleaved;
  float At(uint64_t frame, uint32_t channel) const {
    if (frame >= frames || channel >= channels || channels == 0 || frame > (std::numeric_limits<size_t>::max() - channel) / channels)
      throw std::out_of_range("DecodedPcm coordinates are outside the source dimensions.");
    const size_t index = static_cast<size_t>(frame * channels + channel);
    if (index >= interleaved.size()) throw std::out_of_range("DecodedPcm storage is inconsistent with its dimensions.");
    return interleaved[index];
  }
};
}
