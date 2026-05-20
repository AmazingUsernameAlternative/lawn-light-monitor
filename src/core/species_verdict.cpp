#include "species_verdict.h"

namespace lawn_light {

SpeciesThresholds thresholds_for(Species s) {
    switch (s) {
        case Species::Bermuda:      return {20.0f, 25.0f};
        case Species::Buffalograss: return {20.0f, 25.0f};
        case Species::Zoysia:       return {14.0f, 18.0f};
        case Species::StAugustine:  return {8.0f, 12.0f};
        case Species::TallFescue:   return {6.0f, 10.0f};
    }
    return {0.0f, 0.0f};  // unreachable, satisfies the compiler
}

Verdict verdict_for(Species s, float dli_mol_m2) {
    SpeciesThresholds t = thresholds_for(s);
    if (dli_mol_m2 >= t.min_enough_mol_m2)   return Verdict::Enough;
    if (dli_mol_m2 >= t.min_marginal_mol_m2) return Verdict::Marginal;
    return Verdict::NotEnough;
}

std::string_view name_of(Species s) {
    switch (s) {
        case Species::Bermuda:      return "Bermuda";
        case Species::Buffalograss: return "Buffalograss";
        case Species::Zoysia:       return "Zoysia";
        case Species::StAugustine:  return "St. Augustine";
        case Species::TallFescue:   return "Tall Fescue";
    }
    return "Unknown";
}

std::string_view name_of(Verdict v) {
    switch (v) {
        case Verdict::Enough:    return "ENOUGH";
        case Verdict::Marginal:  return "MARGINAL";
        case Verdict::NotEnough: return "NOT ENOUGH";
    }
    return "UNKNOWN";
}

}  // namespace lawn_light