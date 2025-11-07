cmake_minimum_required(VERSION 3.24)
project(timey LANGUAGES CXX)

find_package(date CONFIG REQUIRED)     # date::date date::date-tz
find_package(Catch2 CONFIG REQUIRED)   # Catch2::Catch2 Catch2::Catch2WithMain
find_package(fmt CONFIG REQUIRED)      # fmt::fmt

# ---- Options ----
option(USE_BUNDLED_TZDB "Prefer bundled tzdb at runtime (hybrid strategy)" ON)
if (NOT WIN32)
  set(USE_BUNDLED_TZDB OFF CACHE BOOL "" FORCE)
endif()

# ---- Warnings / features ----
function(timey_strict_warnings target)
  if (MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive- /Zc:preprocessor /Zc:externConstexpr)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
  target_compile_features(${target} PUBLIC cxx_std_20)
endfunction()

# ---- Core library ----
add_library(tc_core_time STATIC
  src/core_time/core_time.cpp
)
target_include_directories(tc_core_time PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(tc_core_time PUBLIC date::date date::date-tz fmt::fmt)
timey_strict_warnings(tc_core_time)

# tzdb provider placeholder
add_library(tc_tzdb_provider INTERFACE)
target_compile_features(tc_tzdb_provider INTERFACE cxx_std_20)

# ---- Tests ----
enable_testing()
add_executable(tc_tests
  tests/toolchain_smoke.test.cpp
  tests/time_utc_roundtrip.test.cpp
  tests/time_ny_roundtrip.test.cpp
)
target_link_libraries(tc_tests PRIVATE Catch2::Catch2WithMain tc_core_time)
add_test(NAME toolchain_smoke COMMAND tc_tests "[smoke]")
add_test(NAME utc_roundtrip  COMMAND tc_tests "[utc]")
add_test(NAME ny_roundtrip   COMMAND tc_tests "[tz][ny]")

# Hybrid tzdb for tests: point TZDIR to assets if present
if (WIN32 AND USE_BUNDLED_TZDB)
  if (EXISTS "${CMAKE_SOURCE_DIR}/assets/zoneinfo/tzdata.zi")
    set(_tz_env "TZDIR=${CMAKE_SOURCE_DIR}/assets/zoneinfo")
    set_tests_properties(toolchain_smoke PROPERTIES ENVIRONMENT "${_tz_env}")
    set_tests_properties(utc_roundtrip  PROPERTIES ENVIRONMENT "${_tz_env}")
    set_tests_properties(ny_roundtrip   PROPERTIES ENVIRONMENT "${_tz_env}")
  endif()
endif()

# ---- CLI executable ----
add_executable(tc_cli src/cli/main.cpp)
target_link_libraries(tc_cli PRIVATE tc_core_time fmt::fmt)
timey_strict_warnings(tc_cli)

# Single, correct default tzdir for Windows builds
if (WIN32)
  target_compile_definitions(tc_cli PRIVATE TIMEY_DEFAULT_TZDIR="${CMAKE_SOURCE_DIR}/assets/zoneinfo")
endif()

# ---- Install rules ----
if (WIN32 AND USE_BUNDLED_TZDB)
  install(DIRECTORY assets/zoneinfo/ DESTINATION zoneinfo
          FILES_MATCHING
            PATTERN "tzdata.zi"
            PATTERN "leap-seconds.list"
            PATTERN "windowsZones.xml"
            PATTERN "version")
endif()
install(TARGETS tc_cli RUNTIME DESTINATION .)