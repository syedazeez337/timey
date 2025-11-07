#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

// Smoke test: toolchain & linkage only
TEST_CASE("toolchain_smoke", "[smoke]") {
    REQUIRE(true);
}