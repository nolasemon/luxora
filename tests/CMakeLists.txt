cmake_minimum_required(VERSION 3.10)
project(LuxoraTesting)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/googletest/googletest/include
)

add_executable(luxora-testing src/main.cpp)

target_link_libraries(luxora-testing
    PRIVATE
    luxora-lib
    gtest
    gtest_main
)

add_test(NAME LuxoraTesting COMMAND luxora-testing)

