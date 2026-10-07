# UI components (P01.7)

`src/ui/components/Components.h` owns small instance-local view models. `AppShell` owns its SETTINGS demonstration models; no component writes plug-in state, files, History, Library, or network data.

- `ButtonModel` exposes normal, hover, pressed, focused, disabled, busy, success, and error state. Disabled and busy states reject activation.
- `TextFieldModel` is a single-line UTF-8 code-point editor with selection, caret movement, Home/End, insert, Backspace/Delete, maximum length, disabled/read-only state, and pure validation. It preserves complete UTF-8 sequences, but does not implement grapheme-cluster segmentation.
- The custom IGraphics demonstration receives ordinary UTF-8 key events. OS Korean IME composition, clipboard copy/paste, mouse-positioned caret, and DAW-host key delivery were not manually verified and are not claimed complete. Shell routing forwards Ctrl/Cmd and Alt combinations rather than treating them as component shortcuts.
- `ListModel` uses stable string identities, supports single/toggle/range selection, clamps vertical scroll, and exposes a `[first,last)` visible range. The renderer only iterates that range; 10,000 synthetic generic rows do not create controls.
- `ModalModel` is a modal state machine. In the demo it confirms only discarding temporary in-memory text; Escape cancels and background controls cannot activate while open.
- `NotificationModel` is nonpersistent/nonblocking and carries severity, text, optional action, and dismiss state. `ProgressModel` distinguishes determinate, indeterminate, completed, error, canceled, and unavailable states; a percentage is shown only for determinate work.

Input precedence is `Modal > Text > Component/List > Shell`. The SETTINGS panel is explicitly a temporary UI-component demonstration, never a job or sample listing. Project-rendered text uses bundled font IDs and existing theme tokens. Keyboard/DAW accessibility beyond headless routing tests remains `NOT TESTED`.

`tokkebi-ui-component-tests` checks component state machines in Debug and Release, including Korean/mixed UTF-8 editing and 0/1/10,000-row lists. It uses explicit runtime checks, not `assert()`.
