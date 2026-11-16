# Operations

This is a single physical device, not a deployed service. "Deploying" means flashing firmware and printing parts.

## Deployment

- **Environments**: the one controller on the wall; a bench setup (laptop + USB) for changes.
- **Deployment Procedure**: open the sketch fresh in Arduino IDE, select Arduino Uno and its port (COM8 on the build laptop), upload, confirm `string lights v2 ready` in the Serial Monitor. For printed parts, slice with the committed profile and upload via Moonraker (`docs/COMMAND_REFERENCE.md`).
- **Approvals / Gates**: `bash tests/run_all.sh` passes and CI is green before tagging a release; physical prints only with the bed clear and the right filament/nozzle loaded.
- **Rollback**: flash the previous sketch from `firmware/archive/`. EEPROM layout is guarded by `EEPROM_MAGIC`; a version with a different layout ignores stale data.

## Monitoring & Observability

- **Logs**: USB serial at 9600 baud (`status:` lines, unmapped IR codes).
- **Metrics**: none.
- **Alerts**: none; the matrix's standby dot shows the controller has power.

## Safe Operations

- **Backup/Restore**: everything is in git; the device's only state is 5 bytes of EEPROM.
- **Maintenance Mode**: unplug power; the box has no other energy source.
- **Stateful Changes**: changing the `Saved` struct requires bumping `EEPROM_MAGIC`.
- **Electrical**: keep D0/D1 free (a wire on D1 blocks uploads). Do not feed the string 5 V directly; it has no current-limit resistor.

## Incident Response

1. Lights dark: check the matrix (dot = off, press S1), then power, then `status` over serial.
2. Upload fails `not in sync`: free D0/D1, replug USB, reselect the port.
3. For anything that damages hardware, write a postmortem from `constitution/templates/docs/INCIDENT_POSTMORTEM.md` into `docs/incidents/` and add TODO items.
