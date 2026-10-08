# UI components (P01.7)

`Components.h` owns pure state models and strict UTF-8 validation. `VisualControls.h` owns reusable project-rendered `ButtonControl`, `TextFieldControl`, `ListViewControl`, `NotificationControl`, and `ProgressControl`; each owns bounds, semantic-token rendering, hit testing and relevant event routing. `AppShell` composes these controls for SETTINGS rather than duplicating their ordinary drawing/hit rules. No component writes plug-in state, files, History, Library, or network data.

- `ButtonModel` exposes normal, hover, pressed, focused, disabled, busy, success, and error state. Disabled and busy states reject activation.
- `TextFieldModel` is a single-line UTF-8 code-point editor with selection, caret movement, Home/End, insert, Backspace/Delete, maximum length, disabled/read-only state, and pure validation. It preserves complete UTF-8 sequences, but does not implement grapheme-cluster segmentation.
- The custom IGraphics demonstration receives ordinary UTF-8 key events and uses the pinned iPlug2 clipboard API for Ctrl/Cmd+C and Ctrl/Cmd+V. OS Korean IME composition, mouse-positioned caret, and DAW-host key delivery were not manually verified and are not claimed complete. Shell routing forwards unrelated Ctrl/Cmd and Alt combinations rather than treating them as component shortcuts.
- `ListModel` uses stable string identities, supports single/toggle/range selection, clamps vertical scroll, and exposes a `[first,last)` visible range. The renderer only iterates that range; 10,000 synthetic generic rows do not create controls.
- `ModalModel` is a modal state machine. In the demo it confirms only discarding temporary in-memory text; Escape cancels and background controls cannot activate while open.
- `NotificationModel` is nonpersistent/nonblocking and carries severity, text, optional action, and dismiss state. `ProgressModel` distinguishes determinate, indeterminate, completed, error, canceled, and unavailable states; a percentage is shown only for determinate work.

Input precedence is `Modal > Text > Component/List > Shell`. The SETTINGS panel is explicitly a temporary UI-component demonstration, never a job or sample listing. Project-rendered text uses bundled font IDs and existing theme tokens. Keyboard/DAW accessibility beyond headless routing tests remains `NOT TESTED`.

The focus order is shell rail/theme, then SETTINGS text, notification button, enabled clear button, and list. Tab/Shift+Tab moves through this order and skips disabled clear; Tab is handled before text insertion. Modal input is captured before shell routing. It starts on Cancel, Tab/Shift+Tab changes between Cancel and Confirm, Enter/Space activates the focused choice, and Escape cancels. Outside modal clicks do nothing. Cancel restores Clear focus; confirmed clearing moves focus to Notification because Clear becomes disabled. Notification dismissal is restricted to its displayed bounds.

`tokkebi-ui-component-tests` checks component state machines in Debug and Release, including Korean/mixed UTF-8 editing and 0/1/10,000-row lists. It uses explicit runtime checks, not `assert()`.
