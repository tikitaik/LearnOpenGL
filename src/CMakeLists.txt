cmake_minimum_required(VERSION 3.5 VERSION 4.0)

project (LearnOpenGL)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 17)

# if i need to debug
#set(CMAKE_FIND_DEBUG_MODE TRUE)

add_compile_options(-Wall)

set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_PATH ${CMAKE_PREFIX_PATH}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(HINT_SRC ${CMAKE_LIBRARY_PATH}/src)

if (APPLE)

    set(HINT_PLATFORM ${CMAKE_LIBRARY_PATH}/mac)
    message(${HINT_PLATFORM})

elseif(WIN32)

    set(HINT_PLATFORM ${CMAKE_LIBRARY_PATH}/win)
    message(${HINT_PLATFORM})

endif()

add_executable(LearnOpenGL main.cpp)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/src)

target_include_directories(LearnOpenGL PUBLIC ${CMAKE_SOURCE_DIR}/include)

# libraries for all platforms
#find_library(
#    GLAD glad 
#    HINTS ${HINT_SRC} ${HINT_PLATFORM}
#    REQUIRED)
#find_library(
#    STB_IMAGE stb_image
#    HINTS ${HINT_SRC} ${HINT_PLATFORM}
#    REQUIRED)
find_library(
    GLFW3 glfw3 REQUIRED
    HINTS ${HINT_SRC} ${HINT_PLATFORM}
    REQUIRED)
find_library(
    ASSIMP NAMES assimp assimp-6
    HINTS ${HINT_SRC} ${HINT_PLATFORM}
    REQUIRED)

target_link_libraries(LearnOpenGL PRIVATE glad)
target_link_libraries(LearnOpenGL PRIVATE stb_image)
target_link_libraries(LearnOpenGL PRIVATE ${GLFW3})
target_link_libraries(LearnOpenGL PRIVATE ${ASSIMP})

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
