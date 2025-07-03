# Repository Guidelines for Developers and Agents

This project welcomes human and AI contributions. Please keep commits focused and
avoid making unrelated changes. Changes should be submitted via pull request.

## Current Tasks

- Extend the HTTP API according to `Tools/integration_plan.md`. Ensure stub
  endpoints behave like the official modules while integration work continues.
- Add build and configuration support for **ESP32‑S2** and **ESP32‑S3** boards.

## Progress

- Remote method endpoints implemented.
- Stub handlers for additional HTTP API endpoints in place.
- Target and price endpoints persist values for later retrieval.
- ESP32-S2 build target available.
- Notification, region and LED endpoints persist settings.
- Timer and program endpoints remember the last supplied parameters.
- Power history endpoints provide placeholder values for compatibility.
- ESP32-S3 build target available.

For questions or larger design changes, open a GitHub issue first.
