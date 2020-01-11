#include "vermilion/vermilion.h"
#include <cmath>
#include <glad/glad.h>
#include <iostream>

#include <GLFW/glfw3.h>
#include <learnopengl/camera.hpp>
#include <learnopengl/shader.hpp>

int width = 800, height = 600;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "mipmap", nullptr, nullptr);
    glfwSetWindowSizeCallback(window, [](GLFWwindow* window_, int width_, int height_) {
        width = width_;
        height = height_;
    });
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // camera
    Camera camera;

    // shader
    Shader shader("./src/06-Textures_And_Framebuffers/mipmap.vs.glsl",
                  "./src/06-Textures_And_Framebuffers/mipmap.fs.glsl");

    // vao, vbo, ibo
    GLuint vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glad_glBindBuffer(GL_ARRAY_BUFFER, vbo);
    static const GLfloat plane_vertices[] = { -20.0f, 0.0f, -50.0f, -20.0f, 0.0f, 50.0f,
                                              20.0f,  0.0f, -50.0f, 20.0f,  0.0f, 50.0f };

    static const GLfloat plane_texcoords[] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };

    static const GLushort plane_indices[] = { 0, 1, 2, 3 };
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices) + sizeof(plane_texcoords), NULL,
                 GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(plane_vertices), plane_vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(plane_vertices), sizeof(plane_texcoords),
                    plane_texcoords);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(plane_vertices));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_indices), plane_indices, GL_STATIC_DRAW);

    // texture
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    uint32_t* data = new uint32_t[64 * 64];
    uint32_t colors[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF00FFFF,
                          0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF };
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 64 * 64; ++j) {
            data[j] = colors[i];
        }
        glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA8, 64 >> i, 64 >> i, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     data);
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 4.5f);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    delete[] data;

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.25f, 0.3f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
        glDisable(GL_CULL_FACE);

        shader.use();

        auto model_matrix =
            glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -60.0f)) *
            glm::rotate(glm::mat4(), glm::radians(7.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        auto projection_matrix =
            glm::perspectiveFov(glm::radians(35.0f), (float)width, (float)height, 0.1f, 700.0f);

        shader.setMat4("tc_rotate", projection_matrix * model_matrix);

        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, tex);

        float t = std::fmodf(glfwGetTime() / 10.0f, 1.0f);
        if (t < 0.25f) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        } else if (t < 0.5f) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        } else if (t < 0.75f) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
