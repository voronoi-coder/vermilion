#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <learnopengl/camera.hpp>
#include <learnopengl/shader.hpp>

#include <vbm.h>

#include <iostream>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const int point_count = 5000;

VBObject object;
Camera camera;

GLuint geometry_vbo;
GLuint geometry_tex;
GLuint render_vao;

GLuint vao[2];
GLuint vbo[2];

static unsigned int seed = 0x13371337;

static inline float random_float()
{
    float res;
    unsigned int tmp;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);

    *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

    return (res - 1.0f);
}

static glm::vec3 random_vector(float minmag = 0.0f, float maxmag = 1.0f)
{
    glm::vec3 randomvec(random_float() * 2.0f - 1.0f, random_float() * 2.0f -1.0f, random_float() * 2.0f -1.0f);
    randomvec = normalize(randomvec);
    randomvec *= (random_float() * (maxmag - minmag) + minmag);

    return randomvec;
}

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "xfb", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    static const char * varyings2[] =
    {
        "world_space_position"
    };
    Shader shader("./src/05-viewing_transformation/render.vs.glsl", "./src/05-viewing_transformation/blue.fs.glsl", nullptr, 1, varyings2);

    object.loadFromVBM("./src/05-viewing_transformation/armadillo_low.vbm", 0, 1, 2);


    glGenBuffers(1, &geometry_vbo);
    glGenTextures(1, &geometry_tex);
    glBindBuffer(GL_TEXTURE_BUFFER, geometry_vbo);
    glBufferData(GL_TEXTURE_BUFFER, 1024 * 1024 * sizeof(vmath::vec4), NULL, GL_DYNAMIC_COPY);
    glBindTexture(GL_TEXTURE_BUFFER, geometry_tex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, geometry_vbo);

    glGenVertexArrays(1, &render_vao);
    glBindVertexArray(render_vao);
    glBindBuffer(GL_ARRAY_BUFFER, geometry_vbo);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    static const char * varyings[] =
    {
        "position_out", "velocity_out"
    };
    Shader particleShader("./src/05-viewing_transformation/update.vs.glsl", "./src/05-viewing_transformation/white.fs.glsl", nullptr, 2, varyings);

    glGenVertexArrays(2, vao);
    glGenBuffers(2, vbo);

    for(int i = 0; i < 2; ++i)
    {
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, vbo[i]);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, point_count * (sizeof(glm::vec4) + sizeof(glm::vec3)), NULL, GL_DYNAMIC_COPY);
        if(i == 0) {
            struct buffer_t {
                glm::vec4 position;
                glm::vec3 velocity;
            } *buffer = (buffer_t *) glad_glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_WRITE_ONLY);
            for(int j = 0; j < point_count; ++j) {
                buffer[j].velocity = random_vector();
                buffer[j].position = glm::vec4(buffer[j].velocity + glm::vec3(-0.5f, 40.0f, 0.0f), 1.0f);
                buffer[j].velocity = glm::vec3(buffer[j].velocity[0], buffer[j].velocity[1] * 0.3f, buffer[j].velocity[2] * 0.3f);
            }
            glad_glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
        }

        glBindVertexArray(vao[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) + sizeof(glm::vec3), NULL);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) + sizeof(glm::vec3), (GLvoid *)sizeof(glm::vec4));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }
    while(!glfwWindowShouldClose(window)) {
        static float q = glfwGetTime();
        static int frame_count = 0;
        
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 1000.0f);
        shader.setMat4("model_matrix", glm::scale(glm::mat4(), glm::vec3(0.3f)));
        shader.setMat4("projection_matrix", projection * glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0, -100.0f)));

        glBindVertexArray(render_vao);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, geometry_vbo);

        glBeginTransformFeedback(GL_TRIANGLES);
        object.render(0, 0);
        glEndTransformFeedback();

        glad_glPointSize(5);
        particleShader.use();
        particleShader.setMat4("model_matrix", glm::mat4());
        particleShader.setMat4("projection_matrix", projection * glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0, -100.0f)));
        particleShader.setInt("triangle_count", object.getVertexCount() / 3);
        float t = glfwGetTime();
        particleShader.setFloat("time_step", (t - q) * 10.0f);
        q = t;

        if ((frame_count & 1) != 0)
        {
            glBindVertexArray(vao[1]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[0]);
        }
        else
        {
            glBindVertexArray(vao[0]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[1]);
        }

        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, std::min(point_count, (frame_count >> 3)));
        glEndTransformFeedback();

        glBindVertexArray(0);

        frame_count++;

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
