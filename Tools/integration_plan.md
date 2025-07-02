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
