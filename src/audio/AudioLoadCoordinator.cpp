#include "AudioLoadCoordinator.h"
#include "AudioDocument.h"
#include <condition_variable>
#include <mutex>
#include <new>
#include <optional>
#include <thread>
#include <utility>
namespace tokkebi::audio { namespace { AudioLoadResult Failure(DecodeErrorCode code,const char* text){AudioLoadResult r;r.error=code;r.diagnostic=text;return r;} }
struct AudioLoadCoordinator::SharedState { std::mutex mutex; std::condition_variable wake; std::optional<std::pair<uint64_t,std::filesystem::path>> pending; std::optional<std::pair<uint64_t,AudioLoadResult>> completed; bool stopping=false,workerAvailable=false; };
AudioLoadCoordinator::AudioLoadCoordinator(DecodeFunction decode):mShared(std::make_shared<SharedState>()),mDecode(std::move(decode)){const auto shared=mShared;const auto function=mDecode;try{mWorker=std::thread([shared,function]{for(;;){std::pair<uint64_t,std::filesystem::path> request;{std::unique_lock lock(shared->mutex);shared->wake.wait(lock,[&]{return shared->stopping||shared->pending.has_value();});if(shared->stopping)return;request=std::move(*shared->pending);shared->pending.reset();}AudioLoadResult result;try{result=function(request.second);}catch(const std::bad_alloc&){result=Failure(DecodeErrorCode::TooLarge,"Local audio decoding ran out of memory.");}catch(const std::length_error&){result=Failure(DecodeErrorCode::TooLarge,"Local audio decoding exceeded a container limit.");}catch(const std::exception&){result=Failure(DecodeErrorCode::DecodeFailed,"Local audio decoding failed unexpectedly.");}catch(...){result=Failure(DecodeErrorCode::DecodeFailed,"Local audio decoding failed with an unknown error.");}std::lock_guard lock(shared->mutex);if(!shared->stopping)shared->completed=std::make_pair(request.first,std::move(result));}});std::lock_guard lock(shared->mutex);shared->workerAvailable=true;}catch(const std::exception&){std::lock_guard lock(shared->mutex);shared->workerAvailable=false;}}
AudioLoadCoordinator::~AudioLoadCoordinator(){{std::lock_guard lock(mShared->mutex);mShared->stopping=true;mShared->pending.reset();mShared->completed.reset();}mShared->wake.notify_one();if(mWorker.joinable())mWorker.join();}
void AudioLoadCoordinator::Begin(const std::filesystem::path& path,AudioDocument& document){document.BeginLoad();const uint64_t generation=++mGeneration;std::lock_guard lock(mShared->mutex);if(!mShared->workerAvailable){document.Complete(Failure(DecodeErrorCode::IoError,"The local audio worker could not be created."));return;}mShared->pending=std::make_pair(generation,path);mShared->wake.notify_one();}
bool AudioLoadCoordinator::ApplyCompleted(AudioDocument& document){std::optional<std::pair<uint64_t,AudioLoadResult>> completed;{std::lock_guard lock(mShared->mutex);completed.swap(mShared->completed);}if(!completed||completed->first!=mGeneration)return false;document.Complete(std::move(completed->second));return true;}
}
