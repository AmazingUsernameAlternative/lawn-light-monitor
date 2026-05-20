#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "conversion.h"

using lawn_light::lux_to_ppfd;
using lawn_light::K_DAYLIGHT_DEFAULT;

TEST_CASE("lux_to_ppfd: default K converts 10000 lux to 185 PPFD") {
    REQUIRE(lux_to_ppfd(10000.0f) == Catch::Approx(185.0f));
}

TEST_CASE("lux_to_ppfd: zero lux yields zero PPFD") {
    REQUIRE(lux_to_ppfd(0.0f) == Catch::Approx(0.0f));
}

TEST_CASE("lux_to_ppfd: custom K overrides the default") {
    // If someone calibrates K to 0.02, 10000 lux should give 200 PPFD.
    REQUIRE(lux_to_ppfd(10000.0f, 0.02f) == Catch::Approx(200.0f));
}

TEST_CASE("lux_to_ppfd: default K matches the documented constant") {
    // Guard against someone accidentally changing the default.
    // If this fails, it's a deliberate decision and the test should be updated.
    REQUIRE(K_DAYLIGHT_DEFAULT == Catch::Approx(0.0185f));
}