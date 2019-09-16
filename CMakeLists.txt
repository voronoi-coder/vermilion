cmake_minimum_required(VERSION 3.0)

project(vermilion VERSION 9.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 3.3 REQUIRED)

# 01
add_executable(triangles src/01-introduction_to_opengl/triangles.cpp deps/glad/src/glad.c)

# 02
add_executable(uniform_block src/02-shader_fundamentals/uniform_block.cpp deps/glad/src/glad.c)

# 03
add_executable(drawcommands src/03-drawing_with_opengl/drawcommands.cpp deps/glad/src/glad.c)
add_executable(primitiverestart src/03-drawing_with_opengl/primitiverestart.cpp deps/glad/src/glad.c)
add_executable(instancing src/03-drawing_with_opengl/instancing.cpp deps/glad/src/glad.c deps/vbm.cpp)
add_executable(instancing2 src/03-drawing_with_opengl/instancing2.cpp deps/glad/src/glad.c deps/vbm.cpp)
add_executable(instancing3 src/03-drawing_with_opengl/instancing3.cpp deps/glad/src/glad.c deps/vbm.cpp)
add_executable(pointsprites src/03-drawing_with_opengl/pointsprites.cpp
    deps/glad/src/glad.c deps/targa.cpp)

target_include_directories(triangles PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include)
target_link_libraries(triangles glfw)
target_include_directories(drawcommands PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include include)
target_link_libraries(drawcommands glfw)
target_include_directories(uniform_block PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include)
target_link_libraries(uniform_block glfw)
target_include_directories(primitiverestart PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include include)
target_link_libraries(primitiverestart glfw)
target_include_directories(instancing PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include ${CMAKE_SOURCE_DIR}/deps include)
target_link_libraries(instancing glfw)
target_include_directories(instancing2 PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include ${CMAKE_SOURCE_DIR}/deps include)
target_link_libraries(instancing2 glfw)
target_include_directories(instancing3 PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include ${CMAKE_SOURCE_DIR}/deps include)
target_link_libraries(instancing3 glfw)
target_include_directories(pointsprites PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include ${CMAKE_SOURCE_DIR}/deps include)
target_link_libraries(pointsprites glfw)
