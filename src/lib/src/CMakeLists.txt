add_library(glad glad/glad.c)
add_library(stb_image stb_image.cpp)

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
