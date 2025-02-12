cmake_minimum_required(VERSION 3.28)

project(generator)


if(MSVC)
    add_compile_options(/MP)				#Use multiple processors when building
    add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
else()
    add_compile_options(-W -Wall -Werror) #All Warnings, all warnings are errors
endif()

set  (SOURCES
        "src/Generator.h"
        "src/Generator.cpp"
        "src/GenerationTasks.h" #currently using std::async instead of coroutines, so this file doesn't do anything
)

source_group("src" FILES ${SOURCES})

add_library( generator ${SOURCES} )

# Include the 'src' directory.
target_include_directories(generator PUBLIC "${PROJECT_SOURCE_DIR}/src")

# libsodium. (installed through vcpkg)
find_package(unofficial-sodium CONFIG REQUIRED)
target_link_libraries(generator PUBLIC unofficial-sodium::sodium)