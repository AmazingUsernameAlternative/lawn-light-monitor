#pragma once

// Core: lux → PPFD conversion.
// Pure function, no hardware, no state. Testable on host.
//
// PPFD (Photosynthetic Photon Flux Density) is what plants actually care
// about. The VEML7700 measures lux (human-eye-weighted illuminance).
// We approximate PPFD as lux * K, where K ≈ 0.0185 for daylight.
//
// This is the system's largest error source (PRD §11). K is intentionally
// editable. Under tree canopy the spectrum shifts and the true K is lower,
// so DLI under shade is overestimated by this conversion.

namespace lawn_light {

// Daylight conversion constant. Edit and document if calibrated.
constexpr float K_DAYLIGHT_DEFAULT = 0.0185f;

// Convert lux to estimated PPFD (µmol·m⁻²·s⁻¹).
float lux_to_ppfd(float lux, float k = K_DAYLIGHT_DEFAULT);

}  // namespace lawn_light
