#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/camera.hpp>
#include <learnopengl/shader.hpp>
#include <vermilion/vermilion.h>

#include "../deps/vbm.h"

GLuint screenWidth = 800, screenHeight = 600;
float aspect = (float)screenHeight / (float)screenWidth;

Camera camera(glm::vec3(0.0f, 0.0, 0.0f));

VBObject object;

void resize_callback(GLFWwindow* window, int width, int height);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "cubemap", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSetWindowSizeCallback(window, resize_callback);

    Shader shader("./src/06-Textures_And_Framebuffers/cubemap.vs.glsl",
                  "./src/06-Textures_And_Framebuffers/cubemap.fs.glsl");
    Shader objectShader("./src/06-Textures_And_Framebuffers/cubemap_object.vs.glsl",
                        "./src/06-Textures_And_Framebuffers/cubemap_object.fs.glsl");

    static const GLfloat cube_vertices[] = {
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f, 1.0f
    };

    static const GLushort cube_indices[] = {
        0, 1, 2, 3, 6, 7, 4, 5, // First strip
        2, 6, 0, 4, 1, 5, 3, 7  // Second strip
    };

    GLuint vao, vbo, ibo, tex;

    // vbo
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // ibo
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // tex
    vglImageData image;
    vglLoadImage("./src/06-Textures_And_Framebuffers/TantolundenCube.dds", &image);
    assert(image.target == GL_TEXTURE_CUBE_MAP);

    glGenTextures(1, &tex);
    glBindTexture(image.target, tex);
    for (int level = 0; level < image.mipLevels; ++level) {
        for (int face = 0; face < 6; ++face) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, image.internalFormat,
                         image.mip[level].width, image.mip[level].height, 0, image.format,
                         image.type,
                         static_cast<GLubyte*>(image.mip[level].data) + image.sliceStride * face);
        }
    }
    glTexParameteriv(image.target, GL_TEXTURE_SWIZZLE_RGBA,
                     reinterpret_cast<const GLint*>(image.swizzle));
    vglUnloadImage(&image);

    object.loadFromVBM("./src/06-Textures_And_Framebuffers/torus.vbm", 0, 1, 2);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.25f, 0.3f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_CULL_FACE);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        // sky box
        shader.use();

        float t = glfwGetTime() / 1000.0f;
        auto rotate_matrix = glm::rotate(glm::mat4(), t * 137.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        auto tc_matrix = glm::perspective(45.f, 1.0f / aspect, 0.1f, 500.f) *
                         camera.GetViewMatrix() 
                         // * rotate_matrix 
                         ;
        shader.setMat4("tc_rotate", tc_matrix);

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, NULL);
        glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, (void*)(8 * sizeof(GLushort)));

        // object
        objectShader.use();

        tc_matrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -120.0f))
            * glm::rotate(glm::mat4(), 80.0f * 3.0f * t, glm::vec3(0.0f, 1.0f, 0.0f))
            * glm::rotate(glm::mat4(), 70.0f * 3.0f * t, glm::vec3(0.0f, 0.0f, 1.0f))
            * camera.GetViewMatrix();
        objectShader.setMat4("mat_mv", tc_matrix);

        tc_matrix = glm::perspective(45.f, 1.0f / aspect, 0.1f, 500.f) * tc_matrix;
        objectShader.setMat4("mat_mvp", tc_matrix);

        glClear(GL_DEPTH_BUFFER_BIT);

        object.render(0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

void resize_callback(GLFWwindow* window, int width, int height) {
    aspect = (float)height / (float)width;
}
