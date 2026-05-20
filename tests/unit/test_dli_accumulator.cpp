#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dli_accumulator.h"

using lawn_light::DLIAccumulator;
using Catch::Approx;

TEST_CASE("DLIAccumulator: fresh accumulator starts at zero") {
    DLIAccumulator acc;
    REQUIRE(acc.current_dli_mol_m2() == 0.0f);
    REQUIRE(acc.completed_days().empty());
    REQUIRE(acc.average_dli_mol_m2() == 0.0f);
}

TEST_CASE("DLIAccumulator: a single sample contributes (ppfd * dt) / 1e6 mol/m2") {
    DLIAccumulator acc;
    // 1000 PPFD for 60 seconds = 60000 umol/m2 = 0.06 mol/m2
    acc.add_sample(1000.0f, 60.0f);
    REQUIRE(acc.current_dli_mol_m2() == Approx(0.06f));
}

TEST_CASE("DLIAccumulator: multiple samples accumulate") {
    DLIAccumulator acc;
    // One hour at constant 1000 PPFD, sampled every 60 seconds = 60 samples.
    for (int i = 0; i < 60; ++i) {
        acc.add_sample(1000.0f, 60.0f);
    }
    // 60 samples * 0.06 mol/m2 = 3.6 mol/m2 for one hour at 1000 PPFD.
    REQUIRE(acc.current_dli_mol_m2() == Approx(3.6f));
}

TEST_CASE("DLIAccumulator: roll_over_day archives current value and resets to zero") {
    DLIAccumulator acc;
    acc.add_sample(1000.0f, 60.0f);  // 0.06 mol/m2
    REQUIRE(acc.current_dli_mol_m2() == Approx(0.06f));

    acc.roll_over_day();

    // After rollover: today resets to zero, yesterday is archived.
    REQUIRE(acc.current_dli_mol_m2() == 0.0f);
    REQUIRE(acc.completed_days().size() == 1);
    REQUIRE(acc.completed_days()[0] == Approx(0.06f));
}

TEST_CASE("DLIAccumulator: multi-day average computes correctly") {
    DLIAccumulator acc;
    // Day 1: ~20 mol/m2
    acc.add_sample(20'000'000.0f, 1.0f);  // 20 mol/m2 in one shot (hypothetical)
    acc.roll_over_day();
    // Day 2: ~30 mol/m2
    acc.add_sample(30'000'000.0f, 1.0f);
    acc.roll_over_day();
    // Day 3: ~10 mol/m2
    acc.add_sample(10'000'000.0f, 1.0f);
    acc.roll_over_day();

    // Average of [20, 30, 10] = 20.
    REQUIRE(acc.completed_days().size() == 3);
    REQUIRE(acc.average_dli_mol_m2() == Approx(20.0f));
}

TEST_CASE("DLIAccumulator: average is zero when no days have completed") {
    DLIAccumulator acc;
    acc.add_sample(1000.0f, 60.0f);
    // Day hasn't rolled over yet — average across completed days is still 0.
    REQUIRE(acc.average_dli_mol_m2() == 0.0f);
}

TEST_CASE("DLIAccumulator: reset wipes both current day and history") {
    DLIAccumulator acc;
    acc.add_sample(1000.0f, 60.0f);
    acc.roll_over_day();
    acc.add_sample(2000.0f, 60.0f);

    acc.reset();

    REQUIRE(acc.current_dli_mol_m2() == 0.0f);
    REQUIRE(acc.completed_days().empty());
}

TEST_CASE("DLIAccumulator: realistic full day at constant 1000 PPFD yields ~86.4 mol/m2") {
    DLIAccumulator acc;
    // 1440 samples * 60s = 86400s = 24 hours at constant 1000 PPFD.
    for (int i = 0; i < 1440; ++i) {
        acc.add_sample(1000.0f, 60.0f);
    }
    // 1000 * 86400 / 1,000,000 = 86.4 mol/m2/day.
    // This is a sunny-day-ish DLI; sanity check the units are right.
    REQUIRE(acc.current_dli_mol_m2() == Approx(86.4f));
}