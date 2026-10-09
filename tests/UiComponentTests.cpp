#include "ui/components/Components.h"
#include "ui/components/VisualControls.h"
#include <iostream>

#if defined(_WIN32)
float GetScaleForHWND(struct HWND__*) { return 1.f; }
#endif

using namespace tokkebi::ui::components;
static int failures = 0;
static void Check(bool value, const char* message) { if (!value) { ++failures; std::cerr << "UiComponentTests failure: " << message << '\n'; } }
int main()
{
  Check(!IsValidUtf8("\xC0\x80") && !IsValidUtf8("\x80") && !IsValidUtf8("\xED\xA0\x80") && !IsValidUtf8("\xF4\x90\x80\x80") && !IsValidUtf8("\xE3\x81"), "strict malformed UTF-8 rejection");
  Check(IsValidUtf8("한글 😀 café"), "valid Korean emoji UTF-8");
  int callbacks = 0; ButtonModel button([&] { ++callbacks; }); Check(button.Activate() && callbacks == 1, "button activates"); button.SetState(ButtonState::Disabled); Check(!button.Activate() && button.ActivationCount() == 1, "disabled button is inert");
  int routedActivations = 0; ButtonControl routedButton("OPEN LOCAL AUDIO", [&] { ++routedActivations; }); routedButton.SetBounds({10.f, 10.f, 100.f, 24.f}); iplug::IKeyPress enter("", iplug::kVK_RETURN); iplug::IKeyPress space("", iplug::kVK_SPACE); Check(routedButton.OnKey(enter) && routedButton.OnKey(space) && routedActivations == 2, "actual button routes Enter and Space once each"); Check(routedButton.OnMouseDown(15.f, 15.f) && routedActivations == 3, "actual button routes mouse activation");
  TextFieldModel input(32); Check(input.Validation() == TextValidation::Empty, "empty validation"); Check(input.Insert("tokkebi 한글😀"), "mixed UTF-8 inserts"); input.MoveLeft(); Check(input.Backspace(), "backspace removes one UTF-8 code point"); Check(input.Insert("글"), "Korean insert"); input.Home(); input.Delete(); Check(IsValidUtf8(input.Text()), "editing preserves UTF-8"); TextFieldModel limited(8); Check(!limited.Insert("123456789"), "maximum length rejects safely"); input.SetReadOnly(true); Check(input.Validation() == TextValidation::ReadOnly && !input.Delete(), "read-only is inert");
  ListModel list; list.SetViewportRows(5); Check(list.VisibleRange().second == 0, "empty list"); list.SetRows({{"one", "one"}}); Check(list.Select("one"), "one row selects"); std::vector<ListRow> hundred; for (int i = 0; i < 100; ++i) hundred.push_back({"id" + std::to_string(i), "synthetic row"}); list.SetRows(std::move(hundred)); Check(list.VisibleRange().second - list.VisibleRange().first <= 5, "100-row list uses bounded visible range"); std::vector<ListRow> tenThousand; for (int i = 0; i < 10000; ++i) tenThousand.push_back({"id" + std::to_string(i), "synthetic row"}); list.SetRows(std::move(tenThousand)); list.Scroll(99999); auto visible = list.VisibleRange(); Check(visible.second - visible.first <= 5 && visible.second == 10000, "10k list uses bounded visible range"); Check(list.Select("id9999") && list.Selected().front() == "id9999", "stable identity selection");
  ModalModel modal; modal.Show(); Check(modal.Open(), "modal opens"); modal.Cancel(); modal.Show(); modal.Confirm(); Check(!modal.Open() && modal.CancelCount() == 1 && modal.ConfirmCount() == 1, "modal confirm cancel");
  NotificationModel notification; notification.Show({NotificationSeverity::Error, "Controlled test error", {}, {}}); Check(notification.Current().has_value(), "notification shows"); notification.Dismiss(); Check(!notification.Current(), "notification dismisses");
  ProgressModel progress; progress.Set(ProgressState::Determinate, 2.); Check(progress.Fraction() == 1., "progress clamps"); progress.Set(ProgressState::Indeterminate); Check(progress.State() == ProgressState::Indeterminate, "indeterminate state");
  FocusRouter router; router.Set(FocusOwner::Text); Check(!router.RoutesToShell(), "text owns arrows"); router.Set(FocusOwner::Modal); Check(router.Owner() == FocusOwner::Modal, "modal has precedence");
  ComponentFocusRouter focus; focus.Set(ComponentFocus::TextField); Check(focus.Advance(false, false, false) == ComponentFocus::NotificationButton, "focus enters next component"); focus.Set(ComponentFocus::NotificationButton); Check(focus.Advance(false, false, false) == ComponentFocus::List, "disabled clear is skipped"); focus.Set(ComponentFocus::List); Check(focus.Advance(false, true, false) == ComponentFocus::None, "last component exits shell"); focus.Set(ComponentFocus::TextField); Check(focus.Advance(true, true, false) == ComponentFocus::None, "first component exits shell backwards");
  focus.Set(ComponentFocus::List); Check(focus.Advance(false, true, true) == ComponentFocus::NotificationAction, "notification action participates when present"); focus.Set(ComponentFocus::NotificationAction); Check(focus.Advance(false, true, true) == ComponentFocus::None, "notification action exits to shell"); TextFieldModel tabText(64); tabText.SetText("도깨비 test 😀"); const auto beforeTab = tabText.Text(); focus.Set(ComponentFocus::TextField); const auto afterTabFocus = focus.Advance(false, false, false); Check(tabText.Text() == beforeTab && tabText.Text().find('\t') == std::string::npos && afterTabFocus == ComponentFocus::NotificationButton, "tab routing preserves mixed UTF-8 text");
  return failures == 0 ? 0 : 1;
}
