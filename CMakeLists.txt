cmake_minimum_required(VERSION 3.28)

# link vcpkg packages.
if (DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
else ()
    include(FetchContent)
    FetchContent_Declare(vcpkg
            GIT_REPOSITORY https://github.com/microsoft/vcpkg/
    )
    FetchContent_MakeAvailable(vcpkg)
    set(VCPKG_ROOT ${vcpkg_SOURCE_DIR})
endif()
message(STATUS "Using VCPKG_ROOT: ${VCPKG_ROOT}")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(PasswordGen)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)

# Enable testing for google tests
enable_testing()
include(CTest)

add_subdirectory(generator)
add_subdirectory(tests)

option(BUILD_TESTS_ONLY "Configure only the tests directory" OFF)

# Conditionally add projects. Used for github build action, as we only need to build/run tests there
if(NOT BUILD_TESTS_ONLY)
    add_subdirectory(gui)
    add_subdirectory(cli)
endif()