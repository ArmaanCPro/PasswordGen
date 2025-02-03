cmake_minimum_required(VERSION 3.30)

project(generator)

if(MSVC)
    add_compile_options(/MP)				#Use multiple processors when building
    #add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
else()
    #add_compile_options(-W -Wall -Werror) #All Warnings, all warnings are errors
endif()

set  (SOURCES
        "src/main.cpp")

source_group("src" FILES ${SOURCES})

add_executable( generator ${SOURCES} 	)

