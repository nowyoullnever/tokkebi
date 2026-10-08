#pragma once
#include "AudioDecoder.h"
namespace tokkebi::audio {
enum class AudioDocumentState { Empty, Loading, Ready, Failed };
class AudioDocument final {
public:
  AudioDocumentState State() const { return mState; } const AudioFileInfo& Info() const { return mInfo; } const DecodedPcm& Pcm() const { return mPcm; } DecodeErrorCode Error() const { return mError; } const std::string& Diagnostic() const { return mDiagnostic; }
  void Clear(); void BeginLoad(); void Complete(AudioLoadResult result); void LoadLocalFile(const std::filesystem::path& path);
private:
  AudioDocumentState mState = AudioDocumentState::Empty; AudioFileInfo mInfo; DecodedPcm mPcm; DecodeErrorCode mError = DecodeErrorCode::None; std::string mDiagnostic;
}; }
