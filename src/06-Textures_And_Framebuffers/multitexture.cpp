#include <cassert>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "learnopengl/shader.hpp"
#include "vermilion/vermilion.h"

const unsigned int WWidth = 800;
const unsigned int WHeight = 600;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WWidth, WHeight, "multitexture", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    Shader shader("./src/06-Textures_And_Framebuffers/multitexture.vs.glsl", "./src/06-Textures_And_Framebuffers/multitexture.fs.glsl");
    GLuint vao, vbo, tex1, tex2;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    static const GLfloat quad_data[] =
    {
         1.0f, -1.0f,
        -1.0f, -1.0f,
        -1.0f, 1.0f,
         1.0f, 1.0f,

         0.0f, 0.0f,
         1.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 1.0f
    };

    /*
     *  (1, 1)          (0, 1)
     *     +-------------+
     *     |             |
     *     |             |
     *     |             |
     *     |             |
     *     |             |
     *     |             |
     *     +-------------+
     *  (1, 0)          (0,0)
     */

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    // error:
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLfloat *)(sizeof(8 * sizeof(GLfloat))));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLfloat *)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // tex1
    vglImageData image;
    vglLoadImage("./src/06-Textures_And_Framebuffers/test.dds", &image);
    assert(image.target == GL_TEXTURE_2D);

    glGenTextures(1, &tex1);
    glBindTexture(image.target, tex1);
    for(int level = 0; level < image.mipLevels; ++level) {
        glTexImage2D(GL_TEXTURE_2D, level, image.internalFormat,
                image.mip[level].width, image.mip[level].height, 0, image.format, image.type, image.mip[level].data);
    }

    glTexParameteriv(image.target, GL_TEXTURE_SWIZZLE_RGBA, reinterpret_cast<const GLint *>(image.swizzle));
    glTexParameteri(image.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    vglUnloadImage(&image);

    // tex2
    vglImageData image2;
    vglLoadImage("./src/06-Textures_And_Framebuffers/test3.dds", &image2);
    assert(image2.target == GL_TEXTURE_2D);

    glGenTextures(1, &tex2);
    glBindTexture(image2.target, tex2);
    for(int level = 0; level < image2.mipLevels; ++level) {
        glTexImage2D(GL_TEXTURE_2D, level, image2.internalFormat,
                image2.mip[level].width, image2.mip[level].height, 0, image2.format, image2.type, image2.mip[level].data);
    }

    glTexParameteriv(image2.target, GL_TEXTURE_SWIZZLE_RGBA, reinterpret_cast<const GLint *>(image2.swizzle));
    glTexParameteri(image2.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    vglUnloadImage(&image2);

    shader.use();
    shader.setInt("tex1", 0);
    shader.setInt("tex2", 1);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.setFloat("time", glfwGetTime());

        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
