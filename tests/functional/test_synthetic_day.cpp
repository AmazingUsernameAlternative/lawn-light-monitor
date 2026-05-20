#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <cmath>

#include "conversion.h"
#include "dli_accumulator.h"
#include "species_verdict.h"

using namespace lawn_light;
using Catch::Approx;

namespace {

// Simulate a day of lux readings as a half-sine wave (zero at midnight,
// peak at noon, zero again at midnight) and feed them through the full
// core pipeline: lux -> PPFD -> DLIAccumulator. Returns the day's DLI.
float simulate_day(float peak_lux, int samples_per_day = 1440) {
    DLIAccumulator acc;
    const float seconds_per_day = 86400.0f;
    const float dt = seconds_per_day / static_cast<float>(samples_per_day);
    const float pi = 3.14159265358979f;

    for (int i = 0; i < samples_per_day; ++i) {
        const float t = static_cast<float>(i) * dt;
        const float wave = std::sin(pi * t / seconds_per_day);
        const float lux = peak_lux * std::max(0.0f, wave);

        const float ppfd = lux_to_ppfd(lux);
        acc.add_sample(ppfd, dt);
    }

    return acc.current_dli_mol_m2();
}

}  // anonymous namespace

TEST_CASE("Synthetic sunny day: peak 80000 lux yields a sunny-day DLI") {
    const float dli = simulate_day(80'000.0f);

    // A clear sunny day in DFW: half-sine integration of 80k-lux peak
    // gives roughly 60 mol/m2 — a realistic high-DLI summer day.
    INFO("Computed DLI: " << dli);
    REQUIRE(dli > 50.0f);
    REQUIRE(dli < 90.0f);
}

TEST_CASE("Synthetic sunny day: every DFW grass species is ENOUGH") {
    const float dli = simulate_day(80'000.0f);

    REQUIRE(verdict_for(Species::Bermuda,      dli) == Verdict::Enough);
    REQUIRE(verdict_for(Species::Buffalograss, dli) == Verdict::Enough);
    REQUIRE(verdict_for(Species::Zoysia,       dli) == Verdict::Enough);
    REQUIRE(verdict_for(Species::StAugustine,  dli) == Verdict::Enough);
    REQUIRE(verdict_for(Species::TallFescue,   dli) == Verdict::Enough);
}

TEST_CASE("Synthetic deep-shade day: only shade-tolerant species survive") {
    // Peak ~5000 lux is a heavily shaded spot under a dense tree.
    const float dli = simulate_day(5'000.0f);
    INFO("Computed DLI: " << dli);

    // Half-sine integration of 5k peak is roughly 4 mol/m2 — very low.
    REQUIRE(dli < 6.0f);

    // Sun-loving grasses fail in deep shade. Tall Fescue (min ENOUGH = 10)
    // also falls short here, so check the species that are clearly out.
    REQUIRE(verdict_for(Species::Bermuda,      dli) == Verdict::NotEnough);
    REQUIRE(verdict_for(Species::Buffalograss, dli) == Verdict::NotEnough);
    REQUIRE(verdict_for(Species::Zoysia,       dli) == Verdict::NotEnough);
}

TEST_CASE("Synthetic partial-shade day: real-world DLI feels plausible") {
    // Peak ~30000 lux: meaningful filtered light, like dappled shade.
    const float dli = simulate_day(30'000.0f);
    INFO("Computed DLI: " << dli);

    // The half-sine integration here gives ~22 mol/m2 — a respectable
    // partial-shade reading. Sanity-check it lands in a plausible range.
    REQUIRE(dli > 15.0f);
    REQUIRE(dli < 35.0f);

    // Tall Fescue (min ENOUGH = 10) should clear this comfortably.
    REQUIRE(verdict_for(Species::TallFescue, dli) == Verdict::Enough);
}

TEST_CASE("Multi-day session: accumulator averages across simulated days") {
    DLIAccumulator acc;

    const float seconds_per_day = 86400.0f;
    const int samples = 1440;
    const float dt = seconds_per_day / static_cast<float>(samples);
    const float pi = 3.14159265358979f;
    auto run_day = [&](float peak_lux) {
        for (int i = 0; i < samples; ++i) {
            const float t = static_cast<float>(i) * dt;
            const float wave = std::sin(pi * t / seconds_per_day);
            const float lux = peak_lux * std::max(0.0f, wave);
            acc.add_sample(lux_to_ppfd(lux), dt);
        }
        acc.roll_over_day();
    };

    run_day(80'000.0f);   // sunny: ~60 mol/m2
    run_day(40'000.0f);   // partial cloud: ~30 mol/m2
    run_day(60'000.0f);   // mostly sunny: ~45 mol/m2

    REQUIRE(acc.completed_days().size() == 3);
    const float avg = acc.average_dli_mol_m2();
    INFO("Computed average DLI: " << avg);

    // Three plausible summer days, average should land in a sunny-ish range.
    REQUIRE(avg > 30.0f);
    REQUIRE(avg < 70.0f);
}