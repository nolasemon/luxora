cmake_minimum_required(VERSION 3.10)
project(LuxoraTesting)
set(CMAKE_BUILD_TYPE Debug)

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

add_executable(debug-test src/debug.cpp)
target_link_libraries(debug-test PRIVATE luxora-lib)

file(COPY ../resources DESTINATION ./)

include(GoogleTest)
gtest_discover_tests(luxora-testing)
