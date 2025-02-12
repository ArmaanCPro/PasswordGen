cmake_minimum_required(VERSION 3.30)

project(tests)

if(MSVC)
    add_compile_options(/MP)				#Use multiple processors when building
    add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
else()
    add_compile_options(-W -Wall -Werror)   #All Warnings, all warnings are errors
endif()

set  (SOURCES
        "src/main.cpp"
        "src/tests.cpp"
        )

source_group("src" FILES ${SOURCES})


add_executable( tests ${SOURCES} )
add_dependencies( tests generator )
target_link_libraries(tests generator)

# google test bullshits. wasn't working with vcpkg so I just decided to use cmake FetchContent
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(
        tests
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)