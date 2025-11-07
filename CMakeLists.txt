cmake_minimum_required(VERSION 3.24)
project(timey LANGUAGES CXX)

find_package(date CONFIG REQUIRED)     # date::date date::date-tz
find_package(Catch2 CONFIG REQUIRED)   # Catch2::Catch2 Catch2::Catch2WithMain
find_package(fmt CONFIG REQUIRED)      # fmt::fmt

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