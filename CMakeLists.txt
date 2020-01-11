cmake_minimum_required(VERSION 3.0)

project(vermilion VERSION 9.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

configure_file(configuration/root_directory.hpp.in configuration/root_directory.hpp)

find_package(glfw3 3.3 REQUIRED)
find_package(glm)

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
    deps/glad/src/glad.c deps/targa.cpp deps/readShader.cpp)

# 04
add_executable(multisample
    src/04-color_pixels_fragments/anti_aliasing_multisampling.cpp
    deps/glad/src/glad.c)
add_executable(offscreenMSAA
    src/04-color_pixels_fragments/offscreen_multisample.cpp
    deps/glad/src/glad.c
    )
# 05
add_executable(xfb src/05-viewing_transformation/xfb.cpp deps/glad/src/glad.c
    deps/vbm.cpp)

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
target_include_directories(multisample PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include ${CMAKE_SOURCE_DIR}/deps include)
target_include_directories(offscreenMSAA PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include ${CMAKE_SOURCE_DIR}/deps include)
target_include_directories(xfb PUBLIC ${CMAKE_SOURCE_DIR}/deps/glad/include ${CMAKE_SOURCE_DIR}/deps include)
target_link_libraries(offscreenMSAA glfw glm)
target_link_libraries(multisample glfw glm)
target_link_libraries(xfb glfw glm)

include_directories(deps/glad/include)
include_directories(deps)
include_directories(include)
# 06
add_executable(texturewrapping ./src/06-Textures_And_Framebuffers/texturewrapping.cpp
    deps/glad/src/glad.c deps/vermilion.cpp deps/v_loadtexture.cpp)
add_executable(multitexture ./src/06-Textures_And_Framebuffers/multitexture.cpp
    deps/glad/src/glad.c deps/vermilion.cpp deps/v_loadtexture.cpp)
add_executable(volumetexturing
    ./src/06-Textures_And_Framebuffers/volumetexturing.cpp deps/glad/src/glad.c
    deps/vermilion.cpp deps/v_loadtexture.cpp)
add_executable(cubemap
    ./src/06-Textures_And_Framebuffers/cubemap.cpp deps/glad/src/glad.c
    deps/vermilion.cpp deps/v_loadtexture.cpp deps/vbm.cpp)
add_executable(texturedpointsprites
    ./src/06-Textures_And_Framebuffers/texturedpointsprites.cpp deps/glad/src/glad.c
    deps/vermilion.cpp deps/v_loadtexture.cpp deps/targa.cpp)
add_executable(mipmap
    ./src/06-Textures_And_Framebuffers/mipmap.cpp deps/glad/src/glad.c
    deps/vermilion.cpp deps/v_loadtexture.cpp)
target_link_libraries(texturewrapping glfw glm)
target_link_libraries(multitexture glfw glm)
target_link_libraries(cubemap glfw glm)
target_link_libraries(volumetexturing glfw glm)
target_link_libraries(texturedpointsprites glfw glm)
target_link_libraries(mipmap glfw glm)
