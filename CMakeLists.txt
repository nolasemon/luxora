cmake_minimum_required(VERSION 3.20)
project(Luxora VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED on)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address -fno-omit-frame-pointer")

add_compile_options(-Wall -Wpedantic -Wextra)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_subdirectory(external/rapidcsv)
add_subdirectory(external/googletest)

add_library(luxora-lib standalone/lib.cpp ${SRC_FILES})
target_include_directories(luxora-lib
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/rapidcsv/src
)

add_executable(luxora-cli standalone/main.cpp ${SRC_FILES})
target_link_libraries(luxora-cli PRIVATE luxora-lib)

enable_testing()
add_subdirectory(tests)
