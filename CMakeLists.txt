cmake_minimum_required(VERSION 3.10)
project(luxora VERSION 0.1.0 LANGUAGES CXX)

include_directories(include)

add_subdirectory(external/rapidcsv)

add_executable(luxora-cli src/main.cpp)
target_link_libraries(luxora-cli PUBLIC rapidcsv)


include(CTest)
enable_testing()

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
