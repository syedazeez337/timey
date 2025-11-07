#include <date/date.h>
#include <date/tz.h>
#include <fmt/format.h>
#include <chrono>
#include <string>
#include <string_view>
#include <iostream>
#include <sstream>
#include <filesystem>

using namespace std::chrono;
namespace fs = std::filesystem;

static void maybe_init_tzdb() {
#if defined(_WIN32) && defined(TIMEY_DEFAULT_TZDIR)
    try {
        fs::path p = fs::path(TIMEY_DEFAULT_TZDIR);
        if (fs::exists(p)) {
            // date::set_install needs std::string
            date::set_install(p.string());
            date::reload_tzdb();
        }
    } catch (...) {
        // ignore
    }
#endif
}

static void print_usage(const char* argv0) {
    fmt::print("Usage:\n");
    fmt::print("  {} --utc \"YYYY-MM-DDThh:mm:ssZ\" --zone \"Area/Location\"\n\n", argv0);
    fmt::print("Example:\n");
    fmt::print("  {} --utc \"2020-01-01T00:00:00Z\" --zone \"America/New_York\"\n", argv0);
}

static bool parse_utc_iso(std::string_view s, sys_seconds& out) {
    if (s.size() < 1 || s.back() != 'Z') return false;
    std::string core{s.substr(0, s.size()-1)};
    std::istringstream iss(core);
    iss.exceptions(std::ios::failbit | std::ios::badbit);
    try {
        sys_time<seconds> tp{};
        iss.clear(); iss.str(core);
        iss >> date::parse("%FT%T", tp);
        out = time_point_cast<seconds>(tp);
        return true;
    } catch (...) {
        try {
            std::istringstream iss2(core);
            sys_time<minutes> tpmin{};
            iss2 >> date::parse("%FT%R", tpmin);
            out = time_point_cast<seconds>(tpmin);
            return true;
        } catch (...) { return false; }
    }
}

int main(int argc, char** argv) {
    if (argc < 5) { print_usage(argv[0]); return 2; }

    std::string utc_arg, zone_arg;
    for (int i = 1; i + 1 < argc; ++i) {
        std::string k = argv[i];
        if (k == "--utc")  { utc_arg  = argv[++i]; continue; }
        if (k == "--zone") { zone_arg = argv[++i]; continue; }
    }
    if (utc_arg.empty() || zone_arg.empty()) { print_usage(argv[0]); return 2; }

    maybe_init_tzdb();

    sys_seconds tp{};
    if (!parse_utc_iso(utc_arg, tp)) {
        fmt::print(stderr, "error: invalid UTC format: {}\n", utc_arg);
        return 3;
    }

    try {
        const date::time_zone* z = date::locate_zone(zone_arg);
        if (!z) { fmt::print(stderr, "error: zone not found: {}\n", zone_arg); return 4; }
        date::zoned_time<seconds> zt{z, tp};
        fmt::print("{}\n", date::format("%F %T %Z %z", zt));
        return 0;
    } catch (const std::exception& e) {
        fmt::print(stderr, "error: {}\n", e.what());
        return 5;
    }
}