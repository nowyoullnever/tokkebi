#pragma once

#include <filesystem>
#include <mutex>
#include <optional>
#include <string>

namespace tokkebi::ui
{
// This object deliberately has no AppShell or IGraphics reference. Native file
// dialog callbacks may retain it after the editor that opened the dialog dies.
class LocalAudioDialogMailbox final
{
public:
  void Publish(std::string selectedPath)
  {
    if (selectedPath.empty())
      return;
    std::lock_guard lock(mMutex);
    if (!mClosed)
      mPendingPath = std::move(selectedPath);
  }

  std::optional<std::filesystem::path> Consume()
  {
    std::optional<std::string> pending;
    {
      std::lock_guard lock(mMutex);
      if (mClosed)
        return std::nullopt;
      pending.swap(mPendingPath);
    }
    if (!pending)
      return std::nullopt;
    return std::filesystem::u8path(*pending);
  }

  void Close()
  {
    std::lock_guard lock(mMutex);
    mClosed = true;
    mPendingPath.reset();
  }

private:
  std::mutex mMutex;
  std::optional<std::string> mPendingPath;
  bool mClosed = false;
};
}
