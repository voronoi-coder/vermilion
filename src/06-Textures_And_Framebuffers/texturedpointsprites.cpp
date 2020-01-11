#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include <learnopengl/camera.hpp>
#include <learnopengl/shader.hpp>

#include <vermilion/vermilion.h>

#define POINT_COUNT 100

int width = 800, height = 600;

static inline float random_float() {
    float res;
    unsigned int tmp;
    static unsigned int seed = 0x13371337;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);

    *((unsigned int*)&res) = (tmp >> 9) | 0x3F800000;

    return (res - 1.0f);
}

void resize_callback(GLFWwindow*, int width, int height);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "texturedpointsprites", nullptr, nullptr);
    glfwSetWindowSizeCallback(window, resize_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // camera
    Camera camera;

    // shader
    Shader shader("./src/06-Textures_And_Framebuffers/texturedpp.vs.glsl",
                  // "./src/06-Textures_And_Framebuffers/texturedpp.fs.glsl");
                  "./src/06-Textures_And_Framebuffers/analyticpp.fs.glsl");

    // vao vbo
    GLuint vao, vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glad_glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, POINT_COUNT * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
    glm::vec4* vertex_positions = (glm::vec4*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (int i = 0; i < POINT_COUNT; ++i) {
        vertex_positions[i] = glm::vec4(random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f,
                                        random_float() * 2.0f - 1.0f, 1.0f);
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // texture
    GLuint tex;

    vglImageData image;
    vglLoadImage("./src/06-Textures_And_Framebuffers/test3.dds", &image);
    assert(image.target == GL_TEXTURE_2D);

    glGenTextures(1, &tex);
    glBindTexture(image.target, tex);
    for (int level = 0; level < image.mipLevels; ++level) {
        glTexImage2D(GL_TEXTURE_2D, level, image.internalFormat, image.mip[level].width,
                     image.mip[level].height, 0, image.format, image.type, image.mip[level].data);
    }

    glTexParameteriv(image.target, GL_TEXTURE_SWIZZLE_RGBA,
                     reinterpret_cast<const GLint*>(image.swizzle));
    glTexParameteri(image.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    vglUnloadImage(&image);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        // enable/disable
        glEnable(GL_CULL_FACE);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        // glad_glPointParameteri(GL_POINT_FADE_THRESHOLD_SIZE, 64);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw
        float t = glfwGetTime() / 1000.0f;
        auto model_matrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f)) *
                            glm::rotate(glm::mat4(), t * 360.f, glm::vec3(0.0f, 1.0f, 0.0f)) *
                            glm::rotate(glm::mat4(), t * 720.f, glm::vec3(0.0f, 0.0f, 1.0f));
        auto projection_matrix =
            glm::perspectiveFov(glm::radians(camera.Zoom), (float)width, (float)height, 1.0f, 8.0f);

        shader.use();
        shader.setMat4("model_matrix", model_matrix);
        shader.setMat4("projection_matrix", projection_matrix);

        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, tex);
        glDrawArrays(GL_POINTS, 0, POINT_COUNT);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

void resize_callback(GLFWwindow*, int width_, int height_) {
    width = width_;
    height = height_;
}
