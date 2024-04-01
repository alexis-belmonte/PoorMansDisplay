cmake_minimum_required(VERSION 3.27)

project(
    PoorMansDisplay
    VERSION     0.1
    LANGUAGES   CXX
)

set(CMAKE_CXX_STANDARD 20)

set(PMD_SOURCE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PMD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PMD_DEMO_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/demo")
set(PMD_CMAKE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

include("${PMD_CMAKE_DIR}/${PROJECT_NAME}Dependencies.cmake")

file(GLOB_RECURSE PMD_SOURCE_FILES "${PMD_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE PMD_INCLUDE_FILES "${PMD_INCLUDE_DIR}/*.hpp")

add_library(PoorMansDisplay SHARED "${PMD_SOURCE_FILES}")
add_library(PoorMansDisplay::PoorMansDisplay ALIAS PoorMansDisplay)

target_include_directories(
    PoorMansDisplay
    PUBLIC
    $<BUILD_INTERFACE:${PMD_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include/pmd>
)

set_target_properties(
    PoorMansDisplay
    PROPERTIES OUTPUT_NAME "poormansdisplay"
)

target_link_libraries(PoorMansDisplay PNG::PNG JPEG::JPEG)

file(GLOB_RECURSE PMD_DEMO_FILES "${PMD_DEMO_DIR}/*.cpp")

foreach(file ${PMD_DEMO_FILES})
    get_filename_component(name "${file}" NAME_WE)
    add_executable(${name} "${file}")
    target_link_libraries(${name} PoorMansDisplay)
endforeach()

configure_file("${PMD_DEMO_DIR}/DVDLogo.png" "${CMAKE_CURRENT_BINARY_DIR}/DVDLogo.png" COPYONLY)
configure_file("${PMD_DEMO_DIR}/Spinner.png" "${CMAKE_CURRENT_BINARY_DIR}/Spinner.png" COPYONLY)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${PMD_CMAKE_DIR}/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    PATH_VARS CMAKE_INSTALL_LIBDIR
)

install(
    TARGETS PoorMansDisplay
    EXPORT  PoorMansDisplay
    LIBRARY
        COMPONENT pmd
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

#install(
#    FILES ${PMD_INCLUDE_FILES}
#    COMPONENT pmd
#    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/pmd"
#)

export(
    EXPORT PoorMansDisplay
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    EXPORT PoorMansDisplay
    NAMESPACE PoorMansDisplay::
    COMPONENT pmd
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
    COMPONENT pmd
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)
