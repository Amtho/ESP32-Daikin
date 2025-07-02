# Project Architecture and Data Flow

This document gives a high level overview of how the Faikin firmware and
associated tools operate.  It is intended for developers (human or AI) who need
a quick understanding of the project layout and data processing pipeline.

## Purpose

Faikin replaces Daikin's official Wi‑Fi module with an ESP32 based controller.
It offers local web control, MQTT messaging and Home Assistant integration
without any cloud dependency.  The firmware also supports BLE temperature
sensors and optional automation logic.

## Directory Layout

- `ESP/` – ESP‑IDF firmware source.  Contains the main application (`Faikin.c`),
  configuration files and shared components.
- `ESP/components/ESP32-RevK/` – Submodule providing common utilities such as
  settings storage, Wi‑Fi/MQTT helpers and web server glue.
- `ESP/ESP32-BLE-Env/` – Library and example code for reading BLE environment
  sensors.  Parts of this library are symlinked into `ESP/main`.
- `Manuals/` – User documentation (setup, controls, advanced usage) and diagrams.
- `Tools/` – Host utilities: database logger, graph generator and protocol
  simulators for development.
- `PCB/` – KiCad board designs and 3D printable case models.

## Firmware Initialisation Flow

1. **`app_main` startup** – defined in `ESP/main/Faikin.c`.  Hardware is
   initialised and the RevK library (`revk_*` functions) is started.
2. **Settings load** – configuration values from NVS and `settings.def` are
   applied.  This covers serial GPIO pins, MQTT host, Wi‑Fi credentials and
   Faikin specific options such as automation parameters.
3. **Protocol detection** – the code probes the connected air conditioner using
   the S21, X50A or CN_WIRED protocols.  Once a working protocol is found it is
   remembered (unless `protofix` prevents changes).
4. **Web and MQTT services** – a lightweight HTTP server is started if
   `webcontrol`/`websettings` are enabled.  MQTT subscriptions are set up so the
   device can receive control commands.  When Home Assistant support is active
   discovery messages are published.
5. **BLE module** – when Bluetooth is compiled in (`CONFIG_BT_NIMBLE_ENABLED`)
   the `bleenv` task tracks BLE temperature sensors for use in automation or
   for remote displays.
6. **Main loop** – the firmware continuously polls the air conditioner.  Incoming
   packets update the internal state structure (`daikin`) and trigger MQTT/web
   updates.  Outgoing commands are sent when settings change or automation rules
   require adjustments.

## Data Flow

```
    BLE Sensor          MQTT broker              Web browser
        \                   ^                       ^
         \                 /                         |
          --> Faikin <---> MQTT client <--> WebSocket/HTTP
                ^
                |
         Daikin aircon (S21/X50/X35/CN_WIRED)
```

1. **Aircon ↔ Faikin** – Serial or wired protocol messages carry temperature
   readings, mode changes and other status values.  `Faikin.c` contains polling
   logic and parsers for each protocol.
2. **Faikin ↔ MQTT** – `mqtt_client_callback` handles incoming commands under the
   `command/<hostname>/` topic.  Status JSON is periodically published to
   `state/` and `Faikin/` topics.  Home Assistant discovery uses additional
   topics as documented in the source.
3. **Faikin ↔ Web UI** – The embedded HTTP server serves `/control` (HTML UI)
   and `/status` (websocket feed) using handlers registered during startup.
4. **BLE Sensors** – When enabled the `bleenv` module decodes advertisements from
   supported sensors.  Values are stored in the `daikin` structure and can be
   forwarded via MQTT.  Automation logic may use these readings instead of the
   aircon’s internal sensor.
5. **Host Tools** – Utilities under `Tools/` consume the MQTT output.  For
   example `faikinlog` logs readings into a MariaDB database and `faikingraph`
   generates graphs from that data.

## Automation Logic

Automation settings (prefixed `auto.` in `settings.def`) allow the device to
maintain a target temperature range.  The main loop predicts temperature trends
and adjusts the set point using the `pushtemp`/`backtemp` parameters.  Automatic
power control can turn the unit on or off when the room drifts outside the
specified band.  External temperature values may come from a BLE sensor or a
separate MQTT topic (`auto.topic`).

## HTTP Compatibility Layer

`Tools/integration_plan.md` lists HTTP endpoints that emulate Daikin’s official
Wi‑Fi modules.  Most currently return `ret=OK` stubs but allow third‑party
software expecting those endpoints to communicate without errors.  The behaviour
is implemented in `Faikin.c` and can be expanded in future.

## Development Utilities

- **Simulators** (`Tools/Simulators/`) provide standalone programs that emulate
  air conditioner protocols.  They are useful for testing the firmware without
  actual hardware.
- **Logging/graphing** (`Tools/faikinlog.c`, `Tools/faikingraph.c`) read the
  MQTT feed and store or visualise operating data using an SQL database.

## Further Reading

See `Manuals/Setup.md`, `Manuals/Controls.md` and `Manuals/Advanced.md` for end
user instructions.  `Manuals/S21.md` documents the reverse engineered S21
protocol used by many Daikin models.
