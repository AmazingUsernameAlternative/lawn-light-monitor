#include <catch2/catch_test_macros.hpp>

#include "species_verdict.h"

using namespace lawn_light;

TEST_CASE("thresholds_for: Bermuda returns 20/25") {
    auto t = thresholds_for(Species::Bermuda);
    REQUIRE(t.min_marginal_mol_m2 == 20.0f);
    REQUIRE(t.min_enough_mol_m2 == 25.0f);
}

TEST_CASE("thresholds_for: St. Augustine has the lowest ENOUGH bar of warm-season grasses") {
    auto stAug = thresholds_for(Species::StAugustine);
    auto bermuda = thresholds_for(Species::Bermuda);
    REQUIRE(stAug.min_enough_mol_m2 < bermuda.min_enough_mol_m2);
}

TEST_CASE("verdict_for: Bermuda boundary conditions") {
    // 20.0 is the min MARGINAL, 25.0 is the min ENOUGH.
    REQUIRE(verdict_for(Species::Bermuda, 19.9f) == Verdict::NotEnough);
    REQUIRE(verdict_for(Species::Bermuda, 20.0f) == Verdict::Marginal);
    REQUIRE(verdict_for(Species::Bermuda, 24.9f) == Verdict::Marginal);
    REQUIRE(verdict_for(Species::Bermuda, 25.0f) == Verdict::Enough);
    REQUIRE(verdict_for(Species::Bermuda, 30.0f) == Verdict::Enough);
}

TEST_CASE("verdict_for: St. Augustine tolerates shade better than Bermuda") {
    // DLI of 15 — too low for Bermuda, plenty for St. Augustine.
    REQUIRE(verdict_for(Species::Bermuda,     15.0f) == Verdict::NotEnough);
    REQUIRE(verdict_for(Species::StAugustine, 15.0f) == Verdict::Enough);
}

TEST_CASE("verdict_for: Tall Fescue has the lowest light requirement") {
    // DLI of 11 — Enough for Tall Fescue, but not for St. Augustine.
    REQUIRE(verdict_for(Species::TallFescue,  11.0f) == Verdict::Enough);
    REQUIRE(verdict_for(Species::StAugustine, 11.0f) == Verdict::Marginal);
}

TEST_CASE("verdict_for: zero DLI is NotEnough for every species") {
    REQUIRE(verdict_for(Species::Bermuda,      0.0f) == Verdict::NotEnough);
    REQUIRE(verdict_for(Species::Buffalograss, 0.0f) == Verdict::NotEnough);
    REQUIRE(verdict_for(Species::Zoysia,       0.0f) == Verdict::NotEnough);
    REQUIRE(verdict_for(Species::StAugustine,  0.0f) == Verdict::NotEnough);
    REQUIRE(verdict_for(Species::TallFescue,   0.0f) == Verdict::NotEnough);
}

TEST_CASE("name_of: species names are human-readable") {
    REQUIRE(name_of(Species::Bermuda) == "Bermuda");
    REQUIRE(name_of(Species::StAugustine) == "St. Augustine");
    REQUIRE(name_of(Species::TallFescue) == "Tall Fescue");
}

TEST_CASE("name_of: verdict names match the PRD serial output format") {
    REQUIRE(name_of(Verdict::Enough) == "ENOUGH");
    REQUIRE(name_of(Verdict::Marginal) == "MARGINAL");
    REQUIRE(name_of(Verdict::NotEnough) == "NOT ENOUGH");
}