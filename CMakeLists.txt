cmake_minimum_required(VERSION 3.24)
project(timey LANGUAGES CXX)

# deps via vcpkg manifest
find_package(date CONFIG REQUIRED)     # date::date, date::date-tz
find_package(Catch2 CONFIG REQUIRED)   # Catch2::Catch2, Catch2::Catch2WithMain

# placeholder libs (no sources yet)
add_library(tc_core_time INTERFACE)
target_compile_features(tc_core_time INTERFACE cxx_std_20)
target_link_libraries(tc_core_time INTERFACE date::date date::date-tz)

add_library(tc_tzdb_provider INTERFACE)
target_compile_features(tc_tzdb_provider INTERFACE cxx_std_20)

enable_testing()