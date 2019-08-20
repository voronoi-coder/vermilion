cmake_minimum_required(VERSION 3.0)

project(vermilion VERSION 9.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 3.3 REQUIRED)

add_executable(triangles src/01-introduction_to_opengl/triangles.cpp deps/glad/src/glad.c)

add_executable(uniform_block src/02-shader_fundamentals/uniform_block.cpp deps/glad/src/glad.c)

add_executable(drawcommands src/03-drawing_with_opengl/drawcommands.cpp deps/glad/src/glad.c)

target_include_directories(triangles PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include)
target_link_libraries(triangles glfw)
target_include_directories(drawcommands PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include include)
target_link_libraries(drawcommands glfw)
target_include_directories(uniform_block PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include)
target_link_libraries(uniform_block glfw)
