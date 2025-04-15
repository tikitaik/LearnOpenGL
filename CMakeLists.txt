cmake_minimum_required( VERSION 3.5 )

project ( prog )


set(CMAKE_C_COMPILER g++)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_executable( prog main.cpp include/glad.c )

add_compile_options(-mmacosx-version-min=15.3)
add_compile_options(--verbose)

find_package( glfw3 REQUIRED )
target_link_libraries(prog PRIVATE glfw )

find_library(COCOA_FRAMEWORK Cocoa)
if(COCOA_FRAMEWORK)
    target_link_libraries(prog PRIVATE ${COCOA_FRAMEWORK})
else()
    message(FATAL_ERROR "Cocoa framework not found")
endif()

find_library(IOKIT_FRAMEWORK IOKit)
if(IOKIT_FRAMEWORK)
    target_link_libraries(prog PRIVATE ${IOKIT_FRAMEWORK})
else()
    message(FATAL_ERROR "IOKit framework not found")
endif()
