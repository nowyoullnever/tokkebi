#include "AudioDocument.h"
#include <utility>
namespace tokkebi::audio {
void AudioDocument::Clear() { mState = AudioDocumentState::Empty; mInfo = {}; mPcm = {}; mError = DecodeErrorCode::None; mDiagnostic.clear(); }
void AudioDocument::BeginLoad() { mState = AudioDocumentState::Loading; mInfo = {}; mPcm = {}; mError = DecodeErrorCode::None; mDiagnostic.clear(); }
void AudioDocument::Complete(AudioLoadResult result) { mInfo = std::move(result.info); mPcm = std::move(result.audio); mError = result.error; mDiagnostic = std::move(result.diagnostic); mState = result.Success() ? AudioDocumentState::Ready : AudioDocumentState::Failed; }
void AudioDocument::LoadLocalFile(const std::filesystem::path& path) { BeginLoad(); Complete(DecodeLocalAudio(path)); }
}
