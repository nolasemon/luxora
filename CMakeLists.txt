cmake_minimum_required(VERSION 3.10)
project(Luxora VERSION 0.1.0 LANGUAGES CXX)

include_directories(include)

add_subdirectory(external/rapidcsv)

add_library(luxora-lib src/lib.cpp)
target_include_directories(luxora-lib
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    rapidcsv/include
)

add_executable(luxora-cli src/main.cpp)

target_link_libraries(luxora-cli PUBLIC rapidcsv)
target_link_libraries(luxora-cli PUBLIC luxora-lib)

enable_testing()
add_subdirectory(tests)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
