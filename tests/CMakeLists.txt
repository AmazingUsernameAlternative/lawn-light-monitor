include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.3
)
FetchContent_MakeAvailable(Catch2)

# Unit tests — exercise core functions in isolation.
add_executable(unit_tests
    unit/test_conversion.cpp
    unit/test_dli_accumulator.cpp
    unit/test_species_verdict.cpp
)
target_link_libraries(unit_tests PRIVATE lawn_core Catch2::Catch2WithMain)

# Functional tests — drive the core with a synthetic day of lux readings
# and assert end-to-end behavior (EP-4).
add_executable(functional_tests
    functional/test_synthetic_day.cpp
)
target_link_libraries(functional_tests PRIVATE lawn_core Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(unit_tests)
catch_discover_tests(functional_tests)
