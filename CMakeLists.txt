cmake_minimum_required(VERSION 3.24)
project(timey LANGUAGES CXX)

find_package(date CONFIG REQUIRED)     # date::date date::date-tz
find_package(Catch2 CONFIG REQUIRED)   # Catch2::Catch2 Catch2::Catch2WithMain
find_package(fmt CONFIG REQUIRED)      # fmt::fmt

option(USE_BUNDLED_TZDB "Prefer bundled tzdb at runtime (hybrid strategy)" ON)
if (NOT WIN32)
  set(USE_BUNDLED_TZDB OFF CACHE BOOL "" FORCE)
endif()

function(timey_strict_warnings target)
  if (MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive- /Zc:preprocessor /Zc:externConstexpr)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
  target_compile_features(${target} PUBLIC cxx_std_20)
endfunction()

add_library(tc_core_time STATIC
  src/core_time/core_time.cpp
)
target_include_directories(tc_core_time PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(tc_core_time PUBLIC date::date date::date-tz fmt::fmt)
timey_strict_warnings(tc_core_time)

add_library(tc_tzdb_provider INTERFACE)
target_compile_features(tc_tzdb_provider INTERFACE cxx_std_20)

enable_testing()
add_executable(tc_tests
  tests/toolchain_smoke.test.cpp
  tests/time_utc_roundtrip.test.cpp
)
target_link_libraries(tc_tests PRIVATE Catch2::Catch2WithMain tc_core_time)

add_test(NAME toolchain_smoke COMMAND tc_tests "[smoke]")
add_test(NAME utc_roundtrip  COMMAND tc_tests "[utc]")

# Hybrid tzdb: on Windows, if assets/zoneinfo exists, run tests with TZDIR pointing there.
if (WIN32 AND USE_BUNDLED_TZDB)
  if (EXISTS "${CMAKE_SOURCE_DIR}/assets/zoneinfo")
    set(_tz_env "TZDIR=${CMAKE_SOURCE_DIR}/assets/zoneinfo")
    set_tests_properties(toolchain_smoke PROPERTIES ENVIRONMENT "${_tz_env}")
    set_tests_properties(utc_roundtrip  PROPERTIES ENVIRONMENT "${_tz_env}")
  endif()
endif()