#include <catch2/catch_all.hpp>
#include <date/date.h>
#include <date/tz.h>
#include <chrono>
#include <string>

TEST_CASE("NYC conversion on 2020-01-01T00:00:00Z", "[tz][ny]") {
    using namespace std::chrono;

    // Point date::tz to our bundled tzdb and reload it.
    date::set_install("C:/progs/timey/assets/zoneinfo");
    date::reload_tzdb();

    const date::sys_seconds tp = date::sys_days{date::year{2020}/date::January/1} + seconds{0};

    const date::time_zone* ny = date::locate_zone("America/New_York");
    REQUIRE(ny != nullptr);

    const auto info = ny->get_info(tp);
    REQUIRE(info.offset == hours{-5});
    REQUIRE(info.abbrev == std::string("EST"));

    const date::zoned_time<seconds> zt{ny, tp};
    const auto local = date::floor<seconds>(zt.get_local_time());
    const auto expected_local =
        date::local_days{date::year{2019}/date::December/31} + hours{19};
    REQUIRE(local == expected_local);

    const std::string s = date::format("%F %T %Z %z", zt);
    REQUIRE(s == "2019-12-31 19:00:00 EST -0500");
}