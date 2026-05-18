#pragma once

#include <string_view>

// Core: per-species DLI verdicts.
// Thresholds per PRD §6, sourced from Texas A&M AgriLife guidance for
// DFW turfgrasses. Editable constants — the engineering lead must confirm
// these against turf science literature (EP-7 decisions doc).

namespace lawn_light {

enum class Species {
    Bermuda,
    Buffalograss,
    Zoysia,
    StAugustine,
    TallFescue,
};

enum class Verdict {
    NotEnough,
    Marginal,
    Enough,
};

// Thresholds: DLI below `min_marginal` = NOT_ENOUGH,
//             below `min_enough` = MARGINAL,
//             at or above `min_enough` = ENOUGH.
struct SpeciesThresholds {
    float min_marginal_mol_m2;  // lower bound of MARGINAL band
    float min_enough_mol_m2;    // lower bound of ENOUGH band
};

SpeciesThresholds thresholds_for(Species s);
Verdict verdict_for(Species s, float dli_mol_m2);

// Human-readable names for serial output.
std::string_view name_of(Species s);
std::string_view name_of(Verdict v);

}  // namespace lawn_light
