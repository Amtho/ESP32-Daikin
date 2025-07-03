# HTTP API Integration Plan

This project originally implemented only a subset of the Daikin HTTP API used by the official wifi modules.  The documentation at <https://github.com/ael-code/daikin-control> describes a larger set of endpoints.

To improve compatibility we added stub handlers for the missing endpoints.  They currently return simple `ret=OK` responses and do not alter device behaviour.  This allows third party clients expecting the original API to communicate without errors.

## Newly exposed endpoints

- `/common/get_remote_method`
- `/common/set_remote_method`
- `/aircon/get_timer`
- `/aircon/set_timer`
- `/aircon/get_price`
- `/aircon/set_price`
- `/aircon/get_target`
- `/aircon/set_target`
- `/aircon/get_program`
- `/aircon/set_program`
- `/aircon/get_scdltimer`
- `/aircon/set_scdltimer`
- `/common/get_notify`
- `/common/set_notify`
- `/common/set_regioncode`
- `/common/set_led`
- `/common/reboot`
- aliases `/aircon/get_year_power` and `/aircon/get_week_power`

These stubs provide a foundation for future development.  Implementing full behaviour will require referencing hardware capabilities and extending the control logic in `Faikin.c`.

**Important:** Only implement endpoints that are known from the official Daikin
modules.  The list above mirrors the API documented at
[daikin-control](https://github.com/Amtho/daikin-control).  Avoid adding new
URLs until they are verified to exist on the real hardware so that third party
clients remain compatible.

## Task breakdown

The [S21 protocol documentation](../Manuals/S21.md) describes how values are
read from and written to the air-con over the serial port.  Each HTTP endpoint
below should eventually translate its parameters into the appropriate S21
messages so that Faikin mirrors the official modules.

1. **Remote method**
   - `/common/get_remote_method` should report the currently configured remote
     access policy (e.g. *home only* or *anywhere*).
   - `/common/set_remote_method` must parse the `method` query parameter and send
     the matching `D` command to update the unit.

2. **Timer handling**
   - `/aircon/get_timer` and `/aircon/set_timer` map to the `D3/G3` payload used
     for timer scheduling.
   - `/aircon/get_program` and `/aircon/set_program` extend this with weekly
     programmes.

3. **Target and pricing**
   - `/aircon/get_target` and `/aircon/set_target` expose the S21 set-point
     (`F1/D1`).
   - `/aircon/get_price` and `/aircon/set_price` manipulate the energy price
     fields where supported.

4. **Notification and region**
   - `/common/get_notify` and `/common/set_notify` should enable or disable
     status push notifications.
   - `/common/set_regioncode` and `/common/set_led` write to the appropriate S21
     flags controlling region and indicator LED behaviour.

5. **Reboot and power history**
   - `/common/reboot` already reboots the ESP32 after returning `ret=OK`.
   - `/aircon/get_year_power` and `/aircon/get_week_power` return energy usage
     statistics derived from the `GM` payload.

6. **Hardware auto-detection**
   - When connecting to a unit for the first time Faikin probes the serial port
     using S21, X50A and CN_WIRED sequences.  Extending this logic should allow
     detection of hardware features (e.g. fan modes) so that HTTP replies always
     reflect the underlying capabilities.

## Progress

- [x] `/common/get_remote_method` and `/common/set_remote_method` store and
  report the current access policy.
- [x] `/common/set_remote_method` accepts numeric values (`0` or `1`) for
  compatibility with some clients.
- [x] Added stub handlers for remaining HTTP endpoints so third-party clients
  receive `ret=OK` responses.
- [x] `/aircon/get_target` and `/aircon/set_target` persist the requested
  temperature.
- [x] `/aircon/get_price` and `/aircon/set_price` retain the energy price
  value for later queries.
- [x] `/common/get_notify` and `/common/set_notify` persist the notification
  enabled state.
- [x] `/common/set_regioncode` and `/common/set_led` store the region code
  and LED preference.
- [x] `/aircon/get_timer`, `/aircon/set_timer`, `/aircon/get_program`,
  `/aircon/set_program` and `/aircon/get_scdltimer`/`set_scdltimer`
  preserve the most recently supplied values.
- [x] Timer, program and scdl timer queries now report the stored
  parameters using their original field names so third-party clients see
  `timer=`, `program=` and `scdltimer=` responses.
- [x] `/aircon/get_year_power` and `/aircon/get_week_power` now return
  placeholder statistics so clients receive expected fields.
- [x] `/common/set_led` now updates the LED state via S21 commands when
  hardware is connected.
