cmake_minimum_required(VERSION 3.30)

project(cli)


if(MSVC)
    add_compile_options(/MP)				#Use multiple processors when building
    #add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
else()
    #add_compile_options(-W -Wall -Werror) #All Warnings, all warnings are errors
endif()

set  (SOURCES
        "src/cli.cpp"
)

source_group("src" FILES ${SOURCES})

add_executable( cli ${SOURCES} )
add_dependencies( cli generator )
target_link_libraries(cli generator)
