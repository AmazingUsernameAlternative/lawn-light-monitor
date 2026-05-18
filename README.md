# Lawn Light Monitor

A weatherproof IoT device that measures sunlight at a fixed spot on a lawn
and tells you, in plain language, whether each common DFW grass species
is getting enough light to thrive there.

[![CI](https://github.com/YOUR_USER/lawn-light-monitor/actions/workflows/ci.yml/badge.svg)](https://github.com/YOUR_USER/lawn-light-monitor/actions)

> Status: **early development** · Built for BitWize LLC per the project PRD.

## What it does

Light at a spot changes minute-to-minute (clouds, sun angle, leaves moving
overhead). A single reading can't answer the question that actually matters:
*does this patch of ground get enough light, over time, to support the grass
I want to plant there?* This device sits in place for days, measures light
once a minute, and computes the **Daily Light Integral (DLI)** — the total
plant-usable light a spot receives in a day, in mol·m⁻²·day⁻¹.

It then compares that DLI against published thresholds for each common DFW
turfgrass and tells you `ENOUGH`, `MARGINAL`, or `NOT ENOUGH` for each.

## Architecture

See [`docs/architecture.md`](docs/architecture.md) for the block diagram and
the rationale behind splitting the firmware into a pure **core library**
(testable on any computer) and a thin **platform layer** (the Particle/sensor
glue). This split is what makes the unit tests, functional tests, and CI
build possible without hardware in the loop.

## The DLI concept

Plants don't care about the brightness of any single moment — they care about
the total light they receive over the day. We approximate this as:

```
PPFD  = lux × K          where K ≈ 0.0185 for daylight
DLI   = Σ( PPFD × Δt_seconds ) / 1,000,000
```

`K` is the assumption that hurts most under tree shade — chlorophyll absorbs
red and blue, so canopy-filtered light looks brighter to a lux meter than it
actually is to a plant. We document this honestly and leave `K` editable.
See `docs/decisions.md` for the chosen value and any calibration notes.

## Hardware (BOM)

| Item | Purpose |
|---|---|
| Particle Photon 2 | MCU + WiFi + Particle Cloud |
| SparkFun VEML7700 Qwiic breakout | 16-bit ambient light sensor (0–120k+ lux) |
| Qwiic adapter / cable | I²C bridge between Photon 2 and Qwiic ecosystem |
| LiPo battery + Particle JST connector | Power autonomy ≥7 days |
| Weatherproof enclosure + clear window | IP65-class outdoor housing |
| (Phase 2) SparkFun Qwiic OpenLog | microSD logger, if going local-CSV route |

Final part numbers and links live in `docs/bom.md` once the choices are locked.

## Wiring

TBD — diagram coming with hardware bring-up. Short version: VEML7700 board
talks to Photon 2 over I²C via Qwiic; Photon 2 powered by LiPo through its
JST port; USB used for flashing and MVP serial output.

## Build instructions

### Building the core library and running tests (any computer, no hardware)

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

### Building the Particle firmware

```bash
particle compile photon2 . --saveTo lawn-light-monitor.bin
particle flash --usb lawn-light-monitor.bin
```

CI also builds the firmware binary on every merge to `main` and publishes
it as a release artifact — see the Actions tab.

## Reading the serial output

Once flashed, the device streams to USB serial at **115200 baud, 8N1**. Open
any serial terminal (Particle CLI's `particle serial monitor`, `screen`,
`minicom`, the Arduino IDE monitor) and you'll see:

```
LAWN LIGHT MONITOR (BitWize LLC)
[ live ] 14:32:06  lux 78400  PPFD ~1450 umol/m2/s  today DLI 22.6 (so far)

===== DAY 3 COMPLETE (2026-05-20) =====
  DLI: 31.2 mol/m2/day
  Bermuda ......... ENOUGH    Zoysia .......... ENOUGH
  St. Augustine ... ENOUGH    Tall Fescue ..... ENOUGH
  Buffalograss .... ENOUGH

===== OVER TIME (3 days monitored) =====
  Average DLI: 24.8 mol/m2/day
  Bermuda ......... MARGINAL  Zoysia .......... ENOUGH
  St. Augustine ... ENOUGH    Tall Fescue ..... ENOUGH
  Buffalograss .... MARGINAL
  Verdict: this spot supports St. Augustine, Zoysia, and Tall Fescue;
           Bermuda and Buffalograss are borderline.
```

A live reading streams every minute; daily summaries print at local midnight;
the "OVER TIME" running summary re-prints on a regular cadence so a terminal
that connects mid-session sees accumulated results.

## Calibration

`K` (lux → PPFD) defaults to `0.0185` for direct daylight. Under a tree canopy
the true value is lower. To calibrate, take simultaneous lux and reference-PAR
readings at the deployment spot under representative conditions and adjust `K`
in `src/core/conversion.h`. Re-flash. Document the new value and your method
in `docs/decisions.md`.

## Phase 2

Adds long-term untethered data retention. Two paths:
- **Local CSV** — Qwiic OpenLog writes a timestamped CSV to microSD.
- **Cloud** — Daily DLI events publish to Particle Cloud for remote viewing.

See `docs/decisions.md` for which path was chosen and why.

## License

TBD — Apache 2.0 recommended for portfolio-friendly reuse.
