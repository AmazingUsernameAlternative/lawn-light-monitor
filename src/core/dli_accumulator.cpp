#include "dli_accumulator.h"

namespace lawn_light {

void DLIAccumulator::add_sample(float ppfd_umol_m2_s, float delta_seconds) {
    current_day_dli_ += (ppfd_umol_m2_s * delta_seconds) / 1'000'000.0f;
}

float DLIAccumulator::current_dli_mol_m2() const {
    return current_day_dli_;
}

void DLIAccumulator::roll_over_day() {
    completed_.push_back(current_day_dli_);
    current_day_dli_ = 0.0f;
}

float DLIAccumulator::average_dli_mol_m2() const {
    if (completed_.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (float dli : completed_) {
        sum += dli;
    }
    return sum / static_cast<float>(completed_.size());
}

void DLIAccumulator::reset() {
    current_day_dli_ = 0.0f;
    completed_.clear();
}

}  // namespace lawn_light