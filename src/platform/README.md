# Platform layer (the "glue")

This directory holds the **hardware-specific** code: anything that talks to
the Particle Photon 2, the VEML7700 sensor, USB serial, or the Particle Cloud.

These files only build on a Particle target — they pull in Particle Device OS,
the SparkFun VEML7700 library, etc. **Nothing in `src/core/` should ever
`#include` anything from this directory.** That one-way dependency is what
lets us run the core in CI without a Photon 2 attached.

## Planned files

- `main.cpp` — the Particle entry point (`setup()` + `loop()`).
- `veml7700_driver.cpp` — wraps the I²C sensor reads.
- `serial_output.cpp` — formats core data into the human-readable serial
  output shown in PRD §14.
- `sleep_scheduler.cpp` — manages deep-sleep between samples for battery life.
- `time_provider.cpp` — wraps Particle's NTP-synced clock and detects local
  midnight rollover.
- `cloud_publisher.cpp` (Phase 2) — publishes daily DLI events to Particle Cloud.

Everything here should be a thin shim over the core. If a file in this
directory starts containing math or DLI logic, that's a signal it belongs
in `src/core/` instead.
