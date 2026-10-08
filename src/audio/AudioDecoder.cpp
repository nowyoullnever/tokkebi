#include "AudioDecoder.h"

#include <cmath>
#include <cstring>
#include <fstream>
#include <limits>

namespace tokkebi::audio
{
namespace
{
constexpr uint64_t kMaximumInputBytes = 1024ULL * 1024ULL * 1024ULL;
constexpr uint64_t kMaximumFrames = 100000000ULL;

uint16_t ReadU16(const unsigned char* bytes) { return static_cast<uint16_t>(bytes[0] | (static_cast<uint16_t>(bytes[1]) << 8)); }
uint32_t ReadU32(const unsigned char* bytes) { return static_cast<uint32_t>(bytes[0]) | (static_cast<uint32_t>(bytes[1]) << 8) | (static_cast<uint32_t>(bytes[2]) << 16) | (static_cast<uint32_t>(bytes[3]) << 24); }
bool HasTag(const unsigned char* bytes, const char* tag) { return std::memcmp(bytes, tag, 4) == 0; }
void Fail(AudioLoadResult& result, DecodeErrorCode code, const char* message) { result.error = code; result.diagnostic = message; }
}

double FramesToSeconds(uint64_t frames, uint32_t sampleRate) { return sampleRate == 0 ? 0.0 : static_cast<double>(frames) / static_cast<double>(sampleRate); }
uint64_t SecondsToFrames(double seconds, uint32_t sampleRate)
{
  if (!std::isfinite(seconds) || seconds < 0.0 || sampleRate == 0) return 0;
  const double frames = seconds * static_cast<double>(sampleRate);
  return frames > static_cast<double>(std::numeric_limits<uint64_t>::max()) ? 0 : static_cast<uint64_t>(std::llround(frames));
}
double FramesToMilliseconds(uint64_t frames, uint32_t sampleRate) { return FramesToSeconds(frames, sampleRate) * 1000.0; }
uint64_t MillisecondsToFrames(double milliseconds, uint32_t sampleRate) { return !std::isfinite(milliseconds) || milliseconds < 0.0 ? 0 : SecondsToFrames(milliseconds / 1000.0, sampleRate); }

AudioLoadResult DecodeLocalAudio(const std::filesystem::path& path)
{
  AudioLoadResult result;
  result.info.path = path;
  std::error_code error;
  if (!std::filesystem::exists(path, error))
  {
    Fail(result, error ? DecodeErrorCode::IoError : DecodeErrorCode::FileNotFound, error ? "Could not inspect the local path." : "The selected local file does not exist.");
    return result;
  }
  if (!std::filesystem::is_regular_file(path, error) || error)
  {
    Fail(result, DecodeErrorCode::NotRegularFile, "The selected path is not a regular file.");
    return result;
  }
  result.info.fileSize = std::filesystem::file_size(path, error);
  if (error) { Fail(result, DecodeErrorCode::IoError, "Could not determine the selected file size."); return result; }
  if (result.info.fileSize > kMaximumInputBytes) { Fail(result, DecodeErrorCode::TooLarge, "The selected file exceeds the P02.8 decoder safety limit."); return result; }

  std::ifstream input(path, std::ios::binary);
  if (!input) { Fail(result, DecodeErrorCode::IoError, "Could not open the selected local file."); return result; }
  std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  if (input.bad() || bytes.size() != result.info.fileSize) { Fail(result, DecodeErrorCode::IoError, "Could not read the selected local file completely."); return result; }
  if (bytes.size() < 12) { Fail(result, DecodeErrorCode::TruncatedFile, "The selected file is shorter than an audio container header."); return result; }

  if (HasTag(bytes.data(), "FORM") && (HasTag(bytes.data() + 8, "AIFF") || HasTag(bytes.data() + 8, "AIFC")))
  {
    result.info.container = AudioContainer::Aiff; Fail(result, DecodeErrorCode::UnsupportedFormat, "AIFF is recognized but intentionally unsupported in P02.8."); return result;
  }
  if (HasTag(bytes.data(), "fLaC"))
  {
    result.info.container = AudioContainer::Flac; Fail(result, DecodeErrorCode::UnsupportedFormat, "FLAC is recognized but intentionally unsupported in P02.8."); return result;
  }
  if (!HasTag(bytes.data(), "RIFF") || !HasTag(bytes.data() + 8, "WAVE")) { Fail(result, DecodeErrorCode::UnsupportedFormat, "The selected file is not a supported WAV container."); return result; }

  result.info.container = AudioContainer::Wav;
  size_t position = 12, dataOffset = 0;
  uint32_t dataSize = 0, sampleRate = 0;
  uint16_t formatTag = 0, channels = 0, blockAlign = 0, bitsPerSample = 0;
  bool foundFormat = false, foundData = false;
  while (position < bytes.size())
  {
    if (bytes.size() - position < 8) { Fail(result, DecodeErrorCode::TruncatedFile, "A WAV chunk header is truncated."); return result; }
    const uint32_t chunkSize = ReadU32(bytes.data() + position + 4);
    const size_t payloadOffset = position + 8;
    if (chunkSize > bytes.size() - payloadOffset) { Fail(result, DecodeErrorCode::TruncatedFile, "A WAV chunk payload is truncated."); return result; }
    if (HasTag(bytes.data() + position, "fmt ") && !foundFormat)
    {
      if (chunkSize < 16) { Fail(result, DecodeErrorCode::InvalidMetadata, "The WAV format chunk is too short."); return result; }
      formatTag = ReadU16(bytes.data() + payloadOffset); channels = ReadU16(bytes.data() + payloadOffset + 2); sampleRate = ReadU32(bytes.data() + payloadOffset + 4);
      blockAlign = ReadU16(bytes.data() + payloadOffset + 12); bitsPerSample = ReadU16(bytes.data() + payloadOffset + 14); foundFormat = true;
    }
    else if (HasTag(bytes.data() + position, "data") && !foundData) { dataOffset = payloadOffset; dataSize = chunkSize; foundData = true; }
    position = payloadOffset + chunkSize + (chunkSize & 1U);
    if (position > bytes.size()) { Fail(result, DecodeErrorCode::TruncatedFile, "A WAV chunk padding byte is missing."); return result; }
  }
  if (!foundFormat || !foundData) { Fail(result, DecodeErrorCode::CorruptFile, "The WAV container is missing a required format or data chunk."); return result; }
  if (channels == 0 || channels > 32 || sampleRate == 0) { Fail(result, DecodeErrorCode::InvalidMetadata, "The WAV channel count or sample rate is invalid."); return result; }
  if ((formatTag != 1 && formatTag != 3) || (bitsPerSample != 16 && bitsPerSample != 24 && bitsPerSample != 32) || (formatTag == 3 && bitsPerSample != 32))
  { Fail(result, DecodeErrorCode::UnsupportedFormat, "Only PCM 16/24/32-bit and IEEE float 32-bit WAV are supported in P02.8."); return result; }
  const uint64_t bytesPerSample = bitsPerSample / 8U, expectedBlockAlign = static_cast<uint64_t>(channels) * bytesPerSample;
  if (blockAlign != expectedBlockAlign || dataSize % expectedBlockAlign != 0) { Fail(result, DecodeErrorCode::InvalidMetadata, "The WAV block alignment or data length is invalid."); return result; }
  const uint64_t frames = dataSize / expectedBlockAlign;
  if (frames > kMaximumFrames || frames > std::numeric_limits<size_t>::max() / channels) { Fail(result, DecodeErrorCode::TooLarge, "The WAV frame count exceeds the P02.8 decoder safety limit."); return result; }

  result.info.codec = formatTag == 1 ? AudioCodec::Pcm : AudioCodec::IeeeFloat;
  result.info.sourceFormat = bitsPerSample == 16 ? SampleFormat::Int16 : bitsPerSample == 24 ? SampleFormat::Int24 : formatTag == 3 ? SampleFormat::Float32 : SampleFormat::Int32;
  result.info.channels = channels; result.info.sampleRate = sampleRate; result.info.frames = frames; result.info.durationSeconds = FramesToSeconds(frames, sampleRate);
  result.audio = {channels, sampleRate, frames, {}}; result.audio.interleaved.resize(static_cast<size_t>(frames * channels));
  for (uint64_t sample = 0; sample < frames * channels; ++sample)
  {
    const auto* source = bytes.data() + dataOffset + sample * bytesPerSample;
    float decoded = 0.f;
    if (formatTag == 3) { std::memcpy(&decoded, source, sizeof(decoded)); if (!std::isfinite(decoded)) { Fail(result, DecodeErrorCode::InvalidMetadata, "The WAV float payload contains a non-finite sample."); return result; } }
    else if (bitsPerSample == 16) decoded = static_cast<float>(static_cast<int16_t>(ReadU16(source))) / 32768.f;
    else if (bitsPerSample == 24) { int32_t value = static_cast<int32_t>(source[0]) | (static_cast<int32_t>(source[1]) << 8) | (static_cast<int32_t>(source[2]) << 16); if ((value & 0x800000) != 0) value |= ~0xFFFFFF; decoded = static_cast<float>(value) / 8388608.f; }
    else decoded = static_cast<float>(static_cast<int32_t>(ReadU32(source))) / 2147483648.f;
    result.audio.interleaved[static_cast<size_t>(sample)] = decoded;
  }
  return result;
}
}
