#include <catch2/catch_all.hpp>
#include <date/date.h>
#include <string>
#include <chrono>

TEST_CASE("UTC roundtrip formatting is stable", "[utc]") {
    // 2020-01-01 00:00:00 UTC, no tzdb required
    const date::sys_seconds tp =
        date::sys_days{date::year{2020}/date::January/1} + std::chrono::seconds{0};

    // Format sys_time (interpreted as UTC) with date::format
    const std::string s = date::format("%F %T", tp) + " UTC";
    REQUIRE(s == "2020-01-01 00:00:00 UTC");

    // Identity check
    REQUIRE(tp == date::sys_days{date::year{2020}/date::January/1});
}