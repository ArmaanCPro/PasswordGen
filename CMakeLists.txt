cmake_minimum_required(VERSION 3.30)

# link vcpkg packages.
set(VCPKG_ROOT $ENV{VCPKG_ROOT})
message(STATUS "Using VCPKG_ROOT: ${VCPKG_ROOT}")
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(PasswordGen)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(generator)
