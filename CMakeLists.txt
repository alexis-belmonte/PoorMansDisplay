cmake_minimum_required(VERSION 3.27)

project(
    PoorMansDisplay
    VERSION     0.1
    LANGUAGES   CXX
)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(PMD_SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PMD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PMD_SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/event)
set(PMD_DEMO_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/demo)

include_directories(${PMD_INCLUDE_DIR})

file(GLOB_RECURSE PMD_SOURCE_FILES ${PMD_SOURCE_DIR}/*.cpp)
add_library(PoorMansDisplay SHARED ${PMD_SOURCE_FILES})
target_link_libraries(PoorMansDisplay ${CURSES_LIBRARIES})

file(GLOB_RECURSE PMD_DEMO_FILES ${PMD_DEMO_DIR}/*.cpp)
add_executable(demo ${PMD_DEMO_FILES} PoorMansDisplay)
