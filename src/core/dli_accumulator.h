#pragma once

#include <cstddef>
#include <vector>

// Core: DLI accumulator.
// Accumulates PPFD samples over time into a Daily Light Integral
// (mol·m⁻²·day⁻¹), with rollover at local midnight.
//
// DLI = Σ( PPFD × Δt_seconds ) / 1,000,000   (PRD §5)
//
// No hardware, no real time — caller passes elapsed seconds in. This is
// what makes the class testable: a unit test can synthesize a whole day
// of samples in milliseconds.

namespace lawn_light {

class DLIAccumulator {
public:
    DLIAccumulator() = default;

    // Add one PPFD sample covering delta_seconds of elapsed time.
    void add_sample(float ppfd_umol_m2_s, float delta_seconds);

    // DLI accumulated so far for the current (in-progress) day.
    float current_dli_mol_m2() const;

    // Roll the current day's DLI into completed history and start a new day.
    // Called by platform when local midnight crosses.
    void roll_over_day();

    // Completed daily DLI values, oldest first.
    const std::vector<float>& completed_days() const { return completed_; }

    // Average DLI across all completed days. Returns 0 if none yet.
    float average_dli_mol_m2() const;

    // Reset everything (testing convenience).
    void reset();

private:
    float current_day_dli_ = 0.0f;
    std::vector<float> completed_;
};

}  // namespace lawn_light
