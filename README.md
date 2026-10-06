# Now You'll Never — SampleGrabber (working title)

**Status: IMPLEMENTATION AUTHORIZED: P00.2 — SPECIFICATION UNDER ACTIVE REVISION.**

Cross-platform audio capture/collection, sample editing, tagging and library management, integrated with DAWs (planned VST3 / AU / Standalone).

## Canonical instructions

- **[MASTER_BLUEPRINT.md](MASTER_BLUEPRINT.md)**: full product scope, screens, design, typography, web/P2P adapters, audio workflow, sample library, metadata-only History, DAW support, architecture, installer, licensing, tests and release gates.
- **[AGENTS.md](AGENTS.md)**: Codex/PR review rules requiring blueprint-first implementation.

The owner is reviewing and changing the specification. P00.2 only is authorized: it provides the native Standalone bootstrap. VST3/AU, Helper, acquisition, Library, and all later phases remain unapproved.

## Current functionality

P00.2 provides a minimal C++20/CMake iPlug2 Standalone application that opens a text-free graphics surface and exits through the native window lifecycle. It is **not** yet an audio downloader, sample library, editor, or DAW plugin.

- [Build instructions](docs/BUILD.md)
- [Architecture](docs/ARCHITECTURE.md)
- [Dependencies](docs/DEPENDENCIES.md)

This repository is intended to be open source, but the app's final source-code license and product name are **not yet decided**. Third-party tools and fonts have separate license requirements. The fourth font and local font directory contents are pending inspection.
