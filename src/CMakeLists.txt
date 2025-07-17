cmake_minimum_required(VERSION 3.5 VERSION 4.0)

project (LearnOpenGL)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

add_compile_options(-Wall)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (APPLE)

    link_directories(${CMAKE_SOURCE_DIR}/lib/mac)

elseif(WIN32)

    link_directories(${CMAKE_SOURCE_DIR}/lib/win)

endif()

message("source dir is ${CMAKE_SOURCE_DIR}")

add_executable(LearnOpenGL main.cpp)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/src)
target_include_directories(LearnOpenGL PRIVATE ${CMAKE_SOURCE_DIR}/include)

# libraries for all platforms
target_link_libraries(LearnOpenGL PRIVATE glad)
target_link_libraries(LearnOpenGL PRIVATE stb_image)
target_link_libraries(LearnOpenGL PRIVATE glfw3)
target_link_libraries(LearnOpenGL PRIVATE assimp)

if (APPLE)

    add_compile_options(-mmacosx-version-min=15.3)

    # weird mac libraries
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)

    if (COCOA_FRAMEWORK)
        target_link_libraries(LearnOpenGL PRIVATE ${COCOA_FRAMEWORK})
    else()
        message(FATAL_ERROR "Cocoa framework not found")
    endif()

    if (IOKIT_FRAMEWORK)
        target_link_libraries(LearnOpenGL PRIVATE ${IOKIT_FRAMEWORK})
    else()
        message(FATAL_ERROR "IOKit framework not found")
    endif()

elseif (WIN32)

    target_link_libraries(LearnOpenGL PRIVATE opengl32)
    target_link_libraries(LearnOpenGL PRIVATE gdi32)

endif()
