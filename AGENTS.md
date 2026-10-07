# AGENTS.md — Codex / Reviewer Instructions

**PROJECT STATUS: IMPLEMENTATION AUTHORIZED: P01.5 — theme tokens, palette system and visual foundation only. Do not begin P01.6 or later work without explicit owner approval.**

## Mandatory precedence

1. The repository's `MASTER_BLUEPRINT.md` is the canonical product specification. Read it in full before any work.
2. Latest **explicit owner instruction** prevails over old text. Stop and request/record a spec amendment before implementing changes that conflict with the blueprint.
3. A feature request, code comment, README or old prompt does not silently overrule the blueprint.
4. Never assume `OPEN-*` values are final. Do not silently implement the unclear option.

## Scope and change discipline

- Before starting a task, report the exact Requirement IDs (`WEB-007`, `HIS-001`, etc.), file changes planned, tests, and out-of-scope items.
- Work only on the approved phase (`P00` through `P09`). Never begin a later phase without owner approval.
- Favor short, focused branches (`codex/p00-bootstrap`, etc.) and reviewable diffs. Do not force-push main.
- Never fake functionality with silent placeholders, fake successful downloads, fake passing tests or fabricated build logs.
- On any claim of success, provide real build logs, GitHub Actions links or test output. Mark unperformed tests as `NOT TESTED`.
- Preserve user audio and text; do not alter existing DAW project files, qBittorrent torrents/seeding, Soulseek source downloads or cached credentials without explicit consent.

## Visual and font constraints

- Early Cyber/Y2k Futurism, pixelated/thermal/CGI/fractal/dot-matrix/grain style; functional waveform and text must remain readable.
- Use precisely the 12 specified palette colors/tokens in `MASTER_BLUEPRINT.md`.
- Never use the OS default font for in-app UI. Primary 둥근모꼴, long-form/help 리디바탕, auxiliary Galmuri v2.40.4. The user's fourth/local font is `OPEN-FONT-04`, **NOT identified yet**.
- Owner-provided local font folder on owner's machine: `C:\Users\Jung Chan\Desktop\font`. The original folder will be removed after approved files are actually committed. Inspect contents and license before any redistribution; do not claim to have uploaded them unless GitHub and hashes verify. No CDN fonts at runtime.

## Architecture constraints

- Offline Library/History must function without yt-dlp or P2P clients.
- History serialized JSON contains only `source`, `tags`, `description`; no audio, waveform, timestamps or thumbnails.
- VST3/AUv2/Standalone share model/UI. Network/file/download/DB processing belongs in isolated Helper; never block host audio callback.
- URL acquisition and content-use permission are different; no DRM circumvention. Spotify/Apple Music are metadata resolvers, not protected stream extractors.
- Use external qBittorrent WebUI, SoulseekQt folder watcher and slskd API without changing users' other client data.
- Plugin-to-DAW drag is host-dependent; preserve floating helper / reveal file / copy path fallback.
- Sign/notarize distributed macOS binaries appropriately; isolate and license external executables. Installer/uninstaller must preserve Library samples and history.

## Review gate for every change

- Write a PR description referencing Requirement IDs, expected UI/behavior, test commands and logs, screenshots where useful, risks, license effects, and any schema migrations.
- CI compile != tested in FL/Ableton/Logic. Record real host validation separately.
- Regression tests for path safety, histories, metadata, DB and audio outputs are mandatory for their relevant phases.
- If blocked, report what was attempted and what cannot be verified. Do not proceed to the next stage.

## Specification amendment procedure

Update `MASTER_BLUEPRINT.md` section(s) and changelog with owner approval; keep existing IDs stable. If an ID is replaced, deprecate it explicitly. A pending proposal should be marked `OPEN` rather than treated as an approved requirement.
