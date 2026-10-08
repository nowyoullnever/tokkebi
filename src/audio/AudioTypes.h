#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
namespace tokkebi::audio {
enum class AudioContainer { Unknown, Wav, Aiff, Flac };
enum class AudioCodec { Unknown, Pcm, IeeeFloat };
enum class SampleFormat { Unknown, Int16, Int24, Int32, Float32 };
enum class DecodeErrorCode { None, FileNotFound, PermissionDenied, NotRegularFile, UnsupportedFormat, CorruptFile, TruncatedFile, InvalidMetadata, DecodeFailed, TooLarge, IoError };
struct DecodedPcm { uint32_t channels{}; uint32_t sampleRate{}; uint64_t frames{}; std::vector<float> interleaved; float At(uint64_t frame, uint32_t channel) const { return interleaved.at(static_cast<size_t>(frame * channels + channel)); } };
}
