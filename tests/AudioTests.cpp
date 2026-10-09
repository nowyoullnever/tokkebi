#include "audio/AudioDecoder.h"
#include "audio/AudioDocument.h"
#include "audio/AudioLoadCoordinator.h"
#include "ui/LocalAudioDialogMailbox.h"

#include <atomic>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <limits>
#include <memory>
#include <thread>
#include <vector>

namespace
{
int failures = 0;
#define CHECK(value) do { if (!(value)) { std::cerr << "FAILED: " #value "\n"; ++failures; } } while (false)
void U16(std::vector<unsigned char>& b, uint16_t v) { b.push_back(static_cast<unsigned char>(v)); b.push_back(static_cast<unsigned char>(v >> 8)); }
void U32(std::vector<unsigned char>& b, uint32_t v) { for (int i = 0; i < 4; ++i) b.push_back(static_cast<unsigned char>(v >> (i * 8))); }
void Tag(std::vector<unsigned char>& b, const char* t) { b.insert(b.end(), t, t + 4); }
void Patch(std::vector<unsigned char>& b, size_t offset, uint32_t v) { for (int i = 0; i < 4; ++i) b[offset + i] = static_cast<unsigned char>(v >> (i * 8)); }
void Chunk(std::vector<unsigned char>& b, const char* tag, const std::vector<unsigned char>& data) { Tag(b, tag); U32(b, static_cast<uint32_t>(data.size())); b.insert(b.end(), data.begin(), data.end()); if (data.size() & 1U) b.push_back(0); }
std::vector<unsigned char> Fmt(uint16_t f, uint16_t c, uint32_t r, uint16_t bits) { std::vector<unsigned char> b; const auto align = static_cast<uint16_t>(c * (bits / 8)); U16(b, f); U16(b, c); U32(b, r); U32(b, r * align); U16(b, align); U16(b, bits); return b; }
std::vector<unsigned char> Wav(uint16_t f, uint16_t c, uint32_t r, uint16_t bits, const std::vector<unsigned char>& data) { std::vector<unsigned char> b; Tag(b, "RIFF"); U32(b, 0); Tag(b, "WAVE"); Chunk(b, "fmt ", Fmt(f, c, r, bits)); Chunk(b, "data", data); Patch(b, 4, static_cast<uint32_t>(b.size() - 8)); return b; }
void Write(const std::filesystem::path& path, const std::vector<unsigned char>& bytes) { std::ofstream out(path, std::ios::binary); out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size())); }
bool Throws(const tokkebi::audio::DecodedPcm& pcm, uint64_t frame, uint32_t channel) { try { static_cast<void>(pcm.At(frame, channel)); return false; } catch (const std::out_of_range&) { return true; } }
bool ApplyUntil(tokkebi::audio::AudioLoadCoordinator& load, tokkebi::audio::AudioDocument& document) { for (int i = 0; i < 100000; ++i) { if (load.ApplyCompleted(document)) return true; std::this_thread::yield(); } return false; }
tokkebi::audio::AudioLoadResult Success(const std::filesystem::path& path, uint64_t frames) { tokkebi::audio::AudioLoadResult r; r.info.path = path; r.info.frames = frames; r.audio = {1, 48000, frames, std::vector<float>(static_cast<size_t>(frames), .25f)}; return r; }
void UpdateMaximum(std::atomic<int>& maximum, int current) { int observed = maximum.load(); while (observed < current && !maximum.compare_exchange_weak(observed, current)) {} }
}

int main()
{
  using namespace tokkebi::audio;
  const auto root = std::filesystem::temp_directory_path() / "tokkebi-p02-8-3-tests";
  std::error_code ec; std::filesystem::remove_all(root, ec); std::filesystem::create_directories(root, ec);
  const std::vector<unsigned char> p16 {0, 0x80, 0, 0, 0, 0x40, 0xff, 0x7f};
  const auto wav = root / "fixture.wav"; Write(wav, Wav(1, 1, 48000, 16, p16)); const auto decoded = DecodeLocalAudio(wav);
  CHECK(decoded.Success() && decoded.info.frames == 4 && decoded.info.sourceFormat == SampleFormat::Int16);
  CHECK(std::abs(decoded.audio.At(0, 0) + 1.f) < .001f && std::abs(decoded.audio.At(3, 0) - .999969f) < .001f);
  const std::vector<unsigned char> p24 {0,0,0x80,0,0,0,0xff,0xff,0x7f,0,0,0x40}; const auto p24Path = root / "pcm24.wav"; Write(p24Path, Wav(1, 2, 44100, 24, p24)); const auto d24 = DecodeLocalAudio(p24Path); CHECK(d24.Success() && d24.info.channels == 2 && d24.info.frames == 2 && d24.info.sourceFormat == SampleFormat::Int24);
  std::vector<unsigned char> p32; U32(p32, 0x80000000U); U32(p32, 0x40000000U); U32(p32, 0x7fffffffU); const auto p32Path = root / "pcm32.wav"; Write(p32Path, Wav(1, 1, 48000, 32, p32)); const auto d32 = DecodeLocalAudio(p32Path); CHECK(d32.Success() && d32.info.sourceFormat == SampleFormat::Int32 && d32.info.frames == 3);
  float quarter = .25f; std::vector<unsigned char> floatBytes(sizeof(float)); std::memcpy(floatBytes.data(), &quarter, sizeof(float)); const auto floatPath = root / "float.wav"; Write(floatPath, Wav(3, 1, 48000, 32, floatBytes)); CHECK(DecodeLocalAudio(floatPath).Success());
  const auto unicode = root / std::filesystem::u8path(u8"한글.wav"); Write(unicode, Wav(1, 1, 48000, 16, p16)); CHECK(DecodeLocalAudio(unicode).Success());
  const auto wrongExtension = root / "fixture.bin"; Write(wrongExtension, Wav(1, 1, 48000, 16, p16)); CHECK(DecodeLocalAudio(wrongExtension).Success());
  CHECK(DecodeLocalAudio(root / "missing.wav").error == DecodeErrorCode::FileNotFound); CHECK(DecodeLocalAudio(root).error == DecodeErrorCode::NotRegularFile); Write(root / "empty.wav", {}); CHECK(DecodeLocalAudio(root / "empty.wav").error == DecodeErrorCode::TruncatedFile);
  std::vector<unsigned char> truncated {'R','I','F','F',6,0,0,0,'W','A','V','E',0,0}; Write(root / "truncated.wav", truncated); CHECK(DecodeLocalAudio(root / "truncated.wav").error == DecodeErrorCode::TruncatedFile);
  auto noData = Wav(1, 1, 48000, 16, {}); noData.resize(36); Patch(noData, 4, 28); Write(root / "nodata.wav", noData); CHECK(DecodeLocalAudio(root / "nodata.wav").error == DecodeErrorCode::CorruptFile);
  auto badAlign = Wav(1, 1, 48000, 16, p16); badAlign[32] = 1; badAlign[33] = 0; Write(root / "align.wav", badAlign); CHECK(DecodeLocalAudio(root / "align.wav").error == DecodeErrorCode::InvalidMetadata);
  Write(root / "zero.wav", Wav(1, 1, 48000, 16, {})); const auto zero = DecodeLocalAudio(root / "zero.wav"); CHECK(zero.Success() && zero.info.frames == 0);
  Write(root / "unsupported.wav", Wav(1, 1, 48000, 8, {0})); CHECK(DecodeLocalAudio(root / "unsupported.wav").error == DecodeErrorCode::UnsupportedFormat); Write(root / "aiff", {'F','O','R','M',0,0,0,0,'A','I','F','F'}); CHECK(DecodeLocalAudio(root / "aiff").info.container == AudioContainer::Aiff); Write(root / "flac", {'f','L','a','C',0,0,0,0,0,0,0,0}); CHECK(DecodeLocalAudio(root / "flac").info.container == AudioContainer::Flac);
  auto largeRiff = Wav(1, 1, 48000, 16, p16); Patch(largeRiff, 4, 0xffffff00U); Write(root / "riff-big.wav", largeRiff); CHECK(DecodeLocalAudio(root / "riff-big.wav").error == DecodeErrorCode::TruncatedFile);
  auto smallRiff = Wav(1, 1, 48000, 16, p16); Patch(smallRiff, 4, 4); Write(root / "riff-small.wav", smallRiff); CHECK(DecodeLocalAudio(root / "riff-small.wav").error == DecodeErrorCode::CorruptFile);
  std::vector<unsigned char> unknown; Tag(unknown, "RIFF"); U32(unknown, 0); Tag(unknown, "WAVE"); Chunk(unknown, "JUNK", {1,2,3}); Chunk(unknown, "fmt ", Fmt(1, 1, 48000, 16)); Chunk(unknown, "data", p16); Patch(unknown, 4, static_cast<uint32_t>(unknown.size() - 8)); Write(root / "unknown.wav", unknown); CHECK(DecodeLocalAudio(root / "unknown.wav").Success());
  DecodedAllocation allocation; CHECK(CalculateDecodedAllocation(10, 1, allocation) && allocation.sampleCount == 10 && allocation.bytes == 40); CHECK(CalculateDecodedAllocation(10, 2, allocation) && allocation.sampleCount == 20); CHECK(!CalculateDecodedAllocation(std::numeric_limits<uint64_t>::max(), 2, allocation)); CHECK(!CalculateDecodedAllocation(kMaximumDecodedBytes / 4 + 1, 1, allocation)); CHECK(!CalculateDecodedAllocation(1, 0, allocation)); CHECK(decoded.audio.At(0, 0) < 0.f && decoded.audio.At(3, 0) > 0.f && Throws(decoded.audio, decoded.audio.frames, 0) && Throws(decoded.audio, 0, decoded.audio.channels)); CHECK(SecondsToFrames(1.5, 48000) == 72000 && MillisecondsToFrames(250, 48000) == 12000 && SecondsToFrames(-1, 48000) == 0 && SecondsToFrames(std::numeric_limits<double>::infinity(), 48000) == 0 && SecondsToFrames(static_cast<double>(std::numeric_limits<long long>::max()), 1) == 0);

  // Valid float samples precede a NaN: failure cannot retain partially decoded PCM.
  float nan = std::numeric_limits<float>::quiet_NaN(); std::vector<unsigned char> nonFinite(sizeof(float) * 3); std::memcpy(nonFinite.data(), &quarter, sizeof(float)); std::memcpy(nonFinite.data() + sizeof(float), &quarter, sizeof(float)); std::memcpy(nonFinite.data() + sizeof(float) * 2, &nan, sizeof(float)); const auto nonFinitePath = root / "non-finite.wav"; Write(nonFinitePath, Wav(3, 1, 48000, 32, nonFinite)); const auto nonFiniteResult = DecodeLocalAudio(nonFinitePath); CHECK(nonFiniteResult.error == DecodeErrorCode::InvalidMetadata && nonFiniteResult.audio.frames == 0 && nonFiniteResult.audio.interleaved.empty());
  AudioDocument document; document.LoadLocalFile(wav); CHECK(document.State() == AudioDocumentState::Ready); document.LoadLocalFile(root / "missing.wav"); CHECK(document.State() == AudioDocumentState::Failed && document.Pcm().frames == 0 && document.Pcm().interleaved.empty()); AudioLoadResult fakeFailure = Success("fake", 1); fakeFailure.error = DecodeErrorCode::DecodeFailed; fakeFailure.diagnostic = "fake failure"; document.Complete(std::move(fakeFailure)); CHECK(document.State() == AudioDocumentState::Failed && document.Pcm().interleaved.empty() && document.Error() == DecodeErrorCode::DecodeFailed && document.Diagnostic() == "fake failure"); document.Complete(DecodeLocalAudio(nonFinitePath)); CHECK(document.State() == AudioDocumentState::Failed && document.Pcm().interleaved.empty() && document.Error() == DecodeErrorCode::InvalidMetadata);

  // A definitely runs before B is requested. Its completed result is stale while B blocks.
  std::promise<void> aStarted, allowA, bStarted, allowB; auto aStartedFuture = aStarted.get_future(), bStartedFuture = bStarted.get_future(); auto allowAFuture = allowA.get_future().share(), allowBFuture = allowB.get_future().share(); std::atomic<int> active = 0, maximum = 0, aCalls = 0, bCalls = 0;
  auto staleDecoder = [&](const std::filesystem::path& path) { const int now = ++active; UpdateMaximum(maximum, now); if (path.filename() == "A") { ++aCalls; aStarted.set_value(); allowAFuture.wait(); } else { ++bCalls; bStarted.set_value(); allowBFuture.wait(); } --active; return Success(path, path.filename() == "B" ? 2 : 1); };
  AudioDocument staleDocument; { AudioLoadCoordinator load(staleDecoder); load.Begin("A", staleDocument); aStartedFuture.wait(); load.Begin("B", staleDocument); allowA.set_value(); bStartedFuture.wait(); CHECK(!load.ApplyCompleted(staleDocument) && staleDocument.State() == AudioDocumentState::Loading); allowB.set_value(); CHECK(ApplyUntil(load, staleDocument)); CHECK(staleDocument.State() == AudioDocumentState::Ready && staleDocument.Info().path.filename() == "B" && aCalls == 1 && bCalls == 1 && maximum == 1); }

  // B is intentionally skipped when C replaces it while A is running.
  std::promise<void> latestAStarted, latestAllowA, latestCStarted, latestAllowC; auto latestAStartedFuture = latestAStarted.get_future(), latestCStartedFuture = latestCStarted.get_future(); auto latestAllowAFuture = latestAllowA.get_future().share(), latestAllowCFuture = latestAllowC.get_future().share(); std::atomic<int> latestActive = 0, latestMaximum = 0, latestACalls = 0, latestBCalls = 0, latestCCalls = 0;
  auto latestDecoder = [&](const std::filesystem::path& path) { const int now = ++latestActive; UpdateMaximum(latestMaximum, now); if (path.filename() == "A") { ++latestACalls; latestAStarted.set_value(); latestAllowAFuture.wait(); } else if (path.filename() == "B") ++latestBCalls; else { ++latestCCalls; latestCStarted.set_value(); latestAllowCFuture.wait(); } --latestActive; return Success(path, path.filename() == "C" ? 3 : 1); };
  AudioDocument latestDocument; { AudioLoadCoordinator load(latestDecoder); load.Begin("A", latestDocument); latestAStartedFuture.wait(); load.Begin("B", latestDocument); load.Begin("C", latestDocument); latestAllowA.set_value(); latestCStartedFuture.wait(); CHECK(!load.ApplyCompleted(latestDocument) && latestDocument.State() == AudioDocumentState::Loading); latestAllowC.set_value(); CHECK(ApplyUntil(load, latestDocument)); CHECK(latestDocument.State() == AudioDocumentState::Ready && latestDocument.Info().path.filename() == "C" && latestACalls == 1 && latestBCalls == 0 && latestCCalls == 1 && latestMaximum == 1); }

  AudioDocument missingFailure; { AudioLoadCoordinator load([](const std::filesystem::path&) { auto r = Success("missing", 1); r.error = DecodeErrorCode::FileNotFound; return r; }); load.Begin("missing", missingFailure); CHECK(ApplyUntil(load, missingFailure)); } CHECK(missingFailure.State() == AudioDocumentState::Failed && missingFailure.Pcm().interleaved.empty() && missingFailure.Error() == DecodeErrorCode::FileNotFound);
  std::promise<void> exceptionStarted; auto exceptionStartedFuture = exceptionStarted.get_future(); AudioDocument exceptionFailure; { AudioLoadCoordinator load([&](const std::filesystem::path&) -> AudioLoadResult { exceptionStarted.set_value(); throw std::bad_alloc(); }); load.Begin("throw", exceptionFailure); exceptionStartedFuture.wait(); CHECK(ApplyUntil(load, exceptionFailure)); } CHECK(exceptionFailure.State() == AudioDocumentState::Failed && exceptionFailure.Pcm().interleaved.empty() && exceptionFailure.Error() == DecodeErrorCode::TooLarge);
  AudioDocument runtimeFailure; { AudioLoadCoordinator load([](const std::filesystem::path&) -> AudioLoadResult { throw std::runtime_error("test"); }); load.Begin("throw", runtimeFailure); CHECK(ApplyUntil(load, runtimeFailure)); } CHECK(runtimeFailure.State() == AudioDocumentState::Failed && runtimeFailure.Error() == DecodeErrorCode::DecodeFailed && runtimeFailure.Pcm().interleaved.empty());
  std::promise<void> destroyGate; auto destroyFuture = destroyGate.get_future().share(); AudioDocument destroyed; { AudioLoadCoordinator load([&](const std::filesystem::path&) { destroyFuture.wait(); return AudioLoadResult {}; }); load.Begin("destroy", destroyed); destroyGate.set_value(); } CHECK(destroyed.State() == AudioDocumentState::Loading);

  std::weak_ptr<tokkebi::ui::LocalAudioDialogMailbox> weakMailbox; auto ownerMailbox = std::make_shared<tokkebi::ui::LocalAudioDialogMailbox>(); auto callbackMailbox = ownerMailbox; weakMailbox = ownerMailbox; ownerMailbox->Close(); ownerMailbox.reset(); callbackMailbox->Publish("late.wav"); CHECK(!callbackMailbox->Consume().has_value()); callbackMailbox.reset(); CHECK(weakMailbox.expired());
  std::filesystem::remove_all(root, ec); return failures == 0 ? 0 : 1;
}
