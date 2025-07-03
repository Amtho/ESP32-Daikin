# Completed Tasks

The following tasks from `AGENTS.md` have been finished:

- Extended the HTTP API with stub endpoints matching the official modules.
- Added build and configuration support for **ESP32‑S2** and **ESP32‑S3** boards, including default UART pin mappings.
- Target, price and remote method updates are stored persistently and apply immediately.
- Timer, program and schedule timer values are saved across reboots.
- Power history endpoints now poll the power meter before returning data.
- Notification, region and LED settings persist and `/common/set_led` now supports "on"/"off" values.
- `/common/set_notify` mirrors the setting to the physical LED.
- `/common/set_remote_method` now toggles remote control mode.
- Program and schedule timers send the S21 `D4`/`F4` commands so settings
  take effect on the unit.
- Timer endpoints now parse the `timer`, `program` and `scdltimer`
  query parameters and forward them via S21 commands.
- `/common/get_notify` polls the LED state so the returned value reflects the
  current hardware setting.

- `/common/set_remote_method` now forwards the policy via S21 `D8` commands.
- `/common/get_remote_method` polls the current setting using `F8` before replying.
- F8 responses now update the stored `remote_method` value so the HTTP API
  mirrors the air-con setting.
- `/common/set_regioncode` sends the `D9` command so the region change takes effect on the unit.

