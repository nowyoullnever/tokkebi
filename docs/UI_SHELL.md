# UI Shell (P01.6)

## Navigation and state

The shared `AppShell` has exactly six tabs in this order: `WEB`, `P2P`, `INBOX`, `LIBRARY`, `HISTORY`, `SETTINGS`. `WEB` is selected by default. Each shell instance owns its `NavigationState`; selected tab, focus and temporary Light/Dark mode are never process-global and are not saved to disk or plugin state.

Mouse clicks select only visible tab bounds. The theme switch is an explicit compact header control; content clicks never change theme or tab. Tab/Shift+Tab and arrow keys move the visible tab focus; Enter/Space activates it. Ctrl/Cmd and Alt combinations are not consumed. No text-input control exists at this stage, so IME forwarding is not claimed.

## Layout

`ShellLayout` divides the actual IGraphics drawing bounds into header, tab rail, content and status bar. Baselines are 52 px / 42 px / 26 px. Widths at or above 1150 px use the wide class; 760–1149 px use intermediate. Below 760 px, the shell enters compact mode and lays tabs in a two-row, three-column rail so all six remain reachable. The current pinned native editor does not promise OS-level arbitrary resize behavior; calculations and headless invariants cover the supported layout sizes.

## Theme, type and content

All rendering uses P01.5 semantic tokens and the packaged DungGeunMo+, RIDIBatang and Galmuri11Bitmap fonts. Theme switching redraws all four shell regions while preserving the selected tab. Header and inactive tab text use `text.primary` on their semantic surfaces; selected tab labels use `actionText` on `actionDefault`; status labels use `technicalOnAudio` on `audio`.

Each view shows only its section title and a Korean statement of its real implementation status. WEB does not fetch URLs; P2P does not contact clients; INBOX does not inspect files; LIBRARY and HISTORY do not create storage; SETTINGS does not persist preferences. The footer likewise reports that jobs, Helper and Library are not initialized.

## Future integration points

Later stages may replace individual truthful empty views with real page components, add properly scoped status sources, and introduce localized strings. They must retain `TabId`, instance-local state, semantic tokens and the no-persistence/no-audio-callback boundaries until separately approved.
