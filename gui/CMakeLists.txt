cmake_minimum_required(VERSION 3.28)

project(gui)

if(MSVC)
    add_compile_options(/MP)				#Use multiple processors when building
    #add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
else()
    #add_compile_options(-W -Wall -Werror)   #All Warnings, all warnings are errors
endif()

set  (SOURCES
        "src/main.cpp"
        )

source_group("src" FILES ${SOURCES})


add_executable( gui ${SOURCES} )
add_dependencies( gui generator )
target_link_libraries(gui PUBLIC generator)

find_package(wxWidgets CONFIG REQUIRED)
target_link_libraries(gui PRIVATE wx::core wx::base)