#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <learnopengl/shader.hpp>
#include <vermilion/vermilion.h>

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "volumetexturing", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    Shader shader("./src/06-Textures_And_Framebuffers/volumetexturing.vs.glsl",
                  "./src/06-Textures_And_Framebuffers/volumetexturing.fs.glsl");
    GLuint vao, vbo;

    static const GLfloat quad_data[] = { 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,

                                         0.0f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f, 0.0f, 1.0f };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(8 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // tex
    vglImageData image;
    vglLoadImage("./src/06-Textures_And_Framebuffers/cloud.dds", &image);

    assert(image.target == GL_TEXTURE_3D);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(image.target, tex);
    for (int level = 0; level < image.mipLevels; ++level) {
        glTexImage3D(image.target, level, image.internalFormat, image.mip[level].width,
                     image.mip[level].height, image.mip[level].depth, 0, image.format, image.type,
                     image.mip[level].data);
    }

    glTexParameteri(image.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    vglUnloadImage(&image);

    shader.use();
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.6f, 0.7f, 0.9f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);

        float t = glfwGetTime() / 1000.0f;

        auto tc_matrix = glm::rotate(glm::mat4(), t * 170.0f, glm::vec3(1.0f, 0.0f, 0.0f)) *
                         glm::rotate(glm::mat4(), t * 137.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
                         glm::rotate(glm::mat4(), t * 93.0f, glm::vec3(0.0f, 0.0f, 1.0f));

        shader.setMat4("tc_rotate", tc_matrix);

        glBindVertexArray(vao);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDisable(GL_BLEND);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
