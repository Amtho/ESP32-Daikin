# Repository Guidelines for Developers and Agents

This project welcomes human and AI contributions. Please keep commits focused and
avoid making unrelated changes. Changes should be submitted via pull request.

## Current Tasks

Most of the previously stubbed HTTP endpoints now forward their parameters to
the air-con using S21 commands.  Only the pricing API remains purely
configurational.

For details of previously completed tasks see **DONE.md**.

## Pending hardware implementation

The following HTTP API endpoints have basic stub handlers. They store values
for compatibility but do not yet interact with the air-con hardware.  Future
work should update these handlers to translate incoming parameters into the
appropriate S21 or CN_WIRED commands so Faikin mirrors the official Daikin
modules.

- `/aircon/get_price` and `/aircon/set_price`

Implementing these will require referencing the Daikin protocol documents in
`Manuals/S21.md` and extending the control logic in `ESP/main/Faikin.c`.

For questions or larger design changes, open a GitHub issue first.
