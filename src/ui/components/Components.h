#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace tokkebi::ui::components
{
enum class ButtonState { Normal, Hover, Pressed, Focused, Disabled, Busy, Success, Error };

class ButtonModel
{
public:
  explicit ButtonModel(std::function<void()> activate = {}) : mActivate(std::move(activate)) {}
  void SetState(ButtonState state) { mState = state; }
  ButtonState State() const { return mState; }
  bool Activate()
  {
    if (mState == ButtonState::Disabled || mState == ButtonState::Busy)
      return false;
    ++mActivationCount;
    if (mActivate)
      mActivate();
    return true;
  }
  int ActivationCount() const { return mActivationCount; }
private:
  ButtonState mState = ButtonState::Normal;
  std::function<void()> mActivate;
  int mActivationCount = 0;
};

enum class TextValidation { Valid, Empty, TooLong, ReadOnly, Disabled, InvalidUtf8 };

inline bool IsValidUtf8(std::string_view text)
{
  for (std::size_t i = 0; i < text.size();)
  {
    const unsigned char lead = static_cast<unsigned char>(text[i]);
    const std::size_t bytes = lead < 0x80 ? 1 : ((lead & 0xE0) == 0xC0 ? 2 : ((lead & 0xF0) == 0xE0 ? 3 : ((lead & 0xF8) == 0xF0 ? 4 : 0)));
    if (bytes == 0 || i + bytes > text.size() || (bytes == 2 && lead < 0xC2)) return false;
    for (std::size_t part = 1; part < bytes; ++part)
      if ((static_cast<unsigned char>(text[i + part]) & 0xC0) != 0x80) return false;
    i += bytes;
  }
  return true;
}

inline std::vector<std::size_t> Utf8Boundaries(std::string_view text)
{
  std::vector<std::size_t> boundaries {0};
  if (!IsValidUtf8(text)) return boundaries;
  for (std::size_t i = 0; i < text.size();)
  {
    const unsigned char lead = static_cast<unsigned char>(text[i]);
    i += lead < 0x80 ? 1 : ((lead & 0xE0) == 0xC0 ? 2 : ((lead & 0xF0) == 0xE0 ? 3 : 4));
    boundaries.push_back(i);
  }
  return boundaries;
}

class TextFieldModel
{
public:
  explicit TextFieldModel(std::size_t maximumCodePoints = 256) : mMaximum(maximumCodePoints) {}
  const std::string& Text() const { return mText; }
  std::string SelectedText() const
  {
    const auto boundaries = Utf8Boundaries(mText); const auto first = std::min(mCaret, mAnchor), last = std::max(mCaret, mAnchor);
    return mText.substr(boundaries[first], boundaries[last] - boundaries[first]);
  }
  std::size_t Caret() const { return mCaret; }
  void SetDisabled(bool value) { mDisabled = value; }
  void SetReadOnly(bool value) { mReadOnly = value; }
  void SetText(std::string text) { if (IsValidUtf8(text)) { mText = std::move(text); mCaret = Utf8Boundaries(mText).size() - 1; mAnchor = mCaret; } }
  TextValidation Validation() const
  {
    if (mDisabled) return TextValidation::Disabled;
    if (mReadOnly) return TextValidation::ReadOnly;
    if (!IsValidUtf8(mText)) return TextValidation::InvalidUtf8;
    if (mText.empty()) return TextValidation::Empty;
    return Utf8Boundaries(mText).size() - 1 > mMaximum ? TextValidation::TooLong : TextValidation::Valid;
  }
  bool Insert(std::string_view input)
  {
    if (mDisabled || mReadOnly || !IsValidUtf8(input)) return false;
    auto boundaries = Utf8Boundaries(mText);
    const auto inputCount = Utf8Boundaries(input).size() - 1;
    const auto selected = mCaret > mAnchor ? mCaret - mAnchor : mAnchor - mCaret;
    if (boundaries.size() - 1 - selected + inputCount > mMaximum) return false;
    const auto first = std::min(mCaret, mAnchor), last = std::max(mCaret, mAnchor);
    mText.replace(boundaries[first], boundaries[last] - boundaries[first], input);
    mCaret = mAnchor = first + inputCount;
    return true;
  }
  bool Backspace() { return Erase(false); }
  bool Delete() { return Erase(true); }
  void MoveLeft(bool extend = false) { Move(-1, extend); }
  void MoveRight(bool extend = false) { Move(1, extend); }
  void Home(bool extend = false) { mCaret = 0; if (!extend) mAnchor = mCaret; }
  void End(bool extend = false) { mCaret = Utf8Boundaries(mText).size() - 1; if (!extend) mAnchor = mCaret; }
  void SelectAll() { mAnchor = 0; End(true); }
private:
  bool Erase(bool forward)
  {
    if (mDisabled || mReadOnly) return false;
    auto boundaries = Utf8Boundaries(mText);
    auto first = std::min(mCaret, mAnchor), last = std::max(mCaret, mAnchor);
    if (first == last) { if (forward && last < boundaries.size() - 1) ++last; else if (!forward && first > 0) --first; else return false; }
    mText.erase(boundaries[first], boundaries[last] - boundaries[first]); mCaret = mAnchor = first; return true;
  }
  void Move(int direction, bool extend)
  {
    const auto count = Utf8Boundaries(mText).size() - 1;
    if (direction < 0 && mCaret > 0) --mCaret;
    if (direction > 0 && mCaret < count) ++mCaret;
    if (!extend) mAnchor = mCaret;
  }
  std::string mText;
  std::size_t mCaret = 0, mAnchor = 0, mMaximum;
  bool mDisabled = false, mReadOnly = false;
};

struct ListRow { std::string id; std::string label; bool disabled = false; };
class ListModel
{
public:
  void SetRows(std::vector<ListRow> rows) { mRows = std::move(rows); mSelected.erase(std::remove_if(mSelected.begin(), mSelected.end(), [this](const auto& id) { return IndexOf(id) == npos; }), mSelected.end()); ClampScroll(); }
  const std::vector<ListRow>& Rows() const { return mRows; }
  const std::vector<std::string>& Selected() const { return mSelected; }
  static constexpr std::size_t npos = static_cast<std::size_t>(-1);
  std::size_t IndexOf(std::string_view id) const { for (std::size_t i = 0; i < mRows.size(); ++i) if (mRows[i].id == id) return i; return npos; }
  bool Select(std::string_view id, bool toggle = false, bool range = false)
  {
    const auto index = IndexOf(id); if (index == npos || mRows[index].disabled) return false;
    if (range && mAnchor != npos) { mSelected.clear(); const auto lo = std::min(index, mAnchor), hi = std::max(index, mAnchor); for (std::size_t i = lo; i <= hi; ++i) if (!mRows[i].disabled) mSelected.push_back(mRows[i].id); }
    else if (toggle) { const auto at = std::find(mSelected.begin(), mSelected.end(), mRows[index].id); if (at == mSelected.end()) mSelected.push_back(mRows[index].id); else mSelected.erase(at); mAnchor = index; }
    else { mSelected = {mRows[index].id}; mAnchor = index; }
    EnsureVisible(index); return true;
  }
  bool MoveSelection(int direction)
  {
    if (mRows.empty()) return false;
    std::size_t index = mSelected.empty() ? 0 : IndexOf(mSelected.back());
    if (index == npos) index = 0;
    while (true) { const auto next = static_cast<long long>(index) + (direction < 0 ? -1 : 1); if (next < 0 || next >= static_cast<long long>(mRows.size())) return false; index = static_cast<std::size_t>(next); if (!mRows[index].disabled) return Select(mRows[index].id); }
  }
  void SetViewportRows(std::size_t rows) { mViewportRows = rows; ClampScroll(); }
  void Scroll(int delta) { const auto next = static_cast<long long>(mScroll) + delta; mScroll = next < 0 ? 0 : static_cast<std::size_t>(next); ClampScroll(); }
  std::size_t ScrollOffset() const { return mScroll; }
  std::pair<std::size_t, std::size_t> VisibleRange() const { return {mScroll, std::min(mRows.size(), mScroll + mViewportRows)}; }
private:
  void ClampScroll() { const auto maximum = mRows.size() > mViewportRows ? mRows.size() - mViewportRows : 0; mScroll = std::min(mScroll, maximum); }
  void EnsureVisible(std::size_t index) { if (mViewportRows == 0) return; if (index < mScroll) mScroll = index; if (index >= mScroll + mViewportRows) mScroll = index - mViewportRows + 1; ClampScroll(); }
  std::vector<ListRow> mRows; std::vector<std::string> mSelected; std::size_t mAnchor = npos, mScroll = 0, mViewportRows = 4;
};

enum class NotificationSeverity { Information, Success, Warning, Error };
struct Notification { NotificationSeverity severity; std::string message; std::string actionLabel; std::function<void()> action; };
class NotificationModel { public: void Show(Notification value) { mValue = std::move(value); } void Dismiss() { mValue.reset(); } const std::optional<Notification>& Current() const { return mValue; } private: std::optional<Notification> mValue; };
class ModalModel { public: bool Open() const { return mOpen; } void Show() { mOpen = true; } void Cancel() { mOpen = false; ++mCancelCount; } void Confirm() { mOpen = false; ++mConfirmCount; } int ConfirmCount() const { return mConfirmCount; } int CancelCount() const { return mCancelCount; } private: bool mOpen = false; int mConfirmCount = 0, mCancelCount = 0; };
enum class ProgressState { Determinate, Indeterminate, Completed, Error, Canceled, Unavailable };
class ProgressModel { public: void Set(ProgressState state, double fraction = 0.) { mState = state; mFraction = std::clamp(fraction, 0., 1.); } ProgressState State() const { return mState; } double Fraction() const { return mFraction; } private: ProgressState mState = ProgressState::Unavailable; double mFraction = 0.; };
enum class FocusOwner { Shell, Component, Text, Modal };
class FocusRouter { public: FocusOwner Owner() const { return mOwner; } void Set(FocusOwner owner) { mOwner = owner; } bool RoutesToShell() const { return mOwner == FocusOwner::Shell; } private: FocusOwner mOwner = FocusOwner::Shell; };
}
