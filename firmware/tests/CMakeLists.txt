cmake_minimum_required(VERSION 3.16)
project(test_nmea_parser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0
    FIND_PACKAGE_ARGS
)

set(CATCH_CONFIG_FALLBACK_STRINGIFIER ON)
FetchContent_MakeAvailable(catch2)

add_executable(test_nmea_parser test_nmea_parser.cpp)
target_include_directories(test_nmea_parser PRIVATE ${CMAKE_SOURCE_DIR}/../main)
target_link_libraries(test_nmea_parser PRIVATE Catch2::Catch2WithMain)
