# Engineering decisions

PRD §15 leaves several choices to the engineering lead. This document records
those decisions and the reasoning, per EP-7. Update as decisions are made.

## Particle board: Photon 2 (Wi-Fi)

**Chosen:** Particle Photon 2.
**Reason:** Deployment site (Felix's lawn) has Wi-Fi coverage. Photon 2 is
cheaper than Boron, no cellular data plan, and Wi-Fi is sufficient for the
low-rate cloud publishing planned in Phase 2.
**Verify:** Wi-Fi signal strength at the actual mounting spot during the
first site visit. If RSSI is poor or the spot is out of range, revisit and
consider Boron LTE-M.

## Phase 2 path: TBD — leaning Particle Cloud

**Proposed:** Particle Cloud publishing (Option B).
**Reason:** Free tier covers our message rate easily, integrates natively
with the Photon 2, no extra hardware, and remote viewing is more
demonstrable as a portfolio piece than CSV-on-SD.
**Open:** Confirm with sponsor before committing.

## Lux → PPFD constant K

**Default:** `K = 0.0185` (standard daylight value).
**Status:** Not yet calibrated. Documented as a known approximation.
**Tree shade caveat:** Under canopy, true K is lower (canopy filters red and
blue, shifts spectrum green). DLI estimates under shade will run high.
**Optional improvement:** Calibrate against a reference PAR meter at the
deployment spot. Document method and revised K here when done.

## Sampling interval

**Chosen:** 60 seconds.
**Reason:** 1,440 samples per day — plenty for DLI to converge even with
sunfleck noise under a tree. Slow enough to allow deep sleep between
samples for the ≥7-day battery target.

## Per-species DLI thresholds

**Source:** PRD §6 (approximate, from Texas A&M AgriLife guidance).
**Status:** Currently using PRD values verbatim. Engineering lead to confirm
against published turfgrass science before final shipment. Values live in
`src/core/species_verdict.cpp`; update there if revised.

| Species | min MARGINAL (mol/m²/day) | min ENOUGH (mol/m²/day) |
|---|---|---|
| Bermuda | 20 | 25 |
| Buffalograss | 20 | 25 |
| Zoysia | 14 | 18 |
| St. Augustine | 8 | 12 |
| Tall Fescue | 6 | 10 |

(Lower bound of MARGINAL is "slightly below ENOUGH"; below MARGINAL is
NOT_ENOUGH. Final numbers pending confirmation.)

## Power

**TBD.** Felix is providing hardware including battery. Confirm whether the
deployment plan is battery-only, USB-tethered (MVP), or battery + small solar.

## Enclosure and mounting

**TBD.** Provided by sponsor. Confirm IP rating, sensor window material, and
whether it includes a diffuser. Note any window transmittance loss against K.

## Test framework

**Chosen:** Catch2 v3 (via CMake FetchContent).
**Reason:** Single-header simplicity, modern C++17, good CMake integration,
no system install needed in CI.

## CI runner

**Chosen:** GitHub Actions, `ubuntu-latest`.
**Reason:** Free for public repos, standard for this project shape.
