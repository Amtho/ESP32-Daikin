# Completed Tasks

The following tasks from `AGENTS.md` have been finished:

- Extended the HTTP API with stub endpoints matching the official modules.
- Added build and configuration support for **ESP32‑S2** and **ESP32‑S3** boards, including default UART pin mappings.
- Target, price and remote method updates are stored persistently and apply immediately.
- Timer, program and schedule timer values are saved across reboots.
- Program and schedule timer endpoints now issue `DJ` and `DK` commands so the
  air-con updates immediately.
- Power history endpoints return placeholder data for compatibility.
- Notification, region and LED settings persist and `/common/set_led` now supports "on"/"off" values.
- `/common/set_notify` mirrors the setting to the physical LED.
- `/common/set_remote_method` now toggles remote control mode.

