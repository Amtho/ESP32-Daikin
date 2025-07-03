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
- Timer, program and schedule timer settings now stored persistently.
- Target, price and remote method updates apply immediately and are saved.

## Pending hardware implementation

The following HTTP API endpoints have basic stub handlers. They store values
for compatibility but do not yet interact with the air-con hardware.  Future
work should update these handlers to translate incoming parameters into the
appropriate S21 or CN_WIRED commands so Faikin mirrors the official Daikin
modules.

- `/aircon/get_timer` and `/aircon/set_timer`
- `/aircon/get_program` and `/aircon/set_program`
- `/aircon/get_scdltimer` and `/aircon/set_scdltimer`
- `/aircon/get_price` and `/aircon/set_price`
- `/common/get_notify` and `/common/set_notify`
- `/common/get_remote_method` and `/common/set_remote_method`
- `/common/set_regioncode`
- `/aircon/get_year_power` and `/aircon/get_week_power`

Implementing these will require referencing the Daikin protocol documents in
`Manuals/S21.md` and extending the control logic in `ESP/main/Faikin.c`.

For questions or larger design changes, open a GitHub issue first.
