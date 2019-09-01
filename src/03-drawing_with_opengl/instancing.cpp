#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

#include <vbm.h>

static void error_callback(int code, const char *description) {
    std::cerr <<"Error: " << description <<std::endl;
}

static const GLchar *readShader(const char *fn) {
    FILE *infile = fopen(fn, "rb");
    if(!infile) {
        std::cerr << "Unable to open file:" << fn <<"; errno:" << errno <<std::endl;
        return NULL;
    }
    fseek(infile, 0, SEEK_END);
    int len = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    GLchar *source = new GLchar[len + 1];
    fread(source, 1, len, infile);
    source[len] = 0;

    return const_cast<const GLchar *>(source);
}

float aspect;
GLuint program;
GLuint weight_vbo;
GLuint color_vbo;

GLint model_matrix_loc;
GLint projection_matrix_loc;

VBObject object;

#define INSTANCE_COUNT 200

void size_callback(GLFWwindow *window, int width, int height) {
    std::cout << "window size: width, " << width << "; height, " << height << std::endl;
    glViewport(0, 0, width, height);
    aspect = (float)height / (float)width;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    std::cout << "key_callbac key, scancode, action, mods:" << key << ", "<< scancode << ", "<< action << ", "<< mods << std::endl;
}

void char_callback(GLFWwindow *window, unsigned int codepoint) {
    std::cout << "char_callback codepoint:" << codepoint << std::endl;
}

void init() {
const GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *vs_src = readShader("../src/03-drawing_with_opengl/instancing.vs.glsl");
    const GLchar *fs_src = readShader("../src/03-drawing_with_opengl/instancing.fs.glsl");
    glShaderSource(vs, 1, &vs_src, NULL);
    glShaderSource(fs, 1, &fs_src, NULL);
    glCompileShader(vs);
    glCompileShader(fs);

    // GLint compiled;
    // glGetShaderiv( vs, GL_COMPILE_STATUS, &compiled );
    // if ( !compiled ) {
    //     GLsizei len;
    //     glGetShaderiv( vs, GL_INFO_LOG_LENGTH, &len );
    //
    //     GLchar* log = new GLchar[len+1];
    //     glGetShaderInfoLog( vs, len, &len, log );
    //     std::cerr << "Shader compilation failed: " << log << std::endl;
    //     delete [] log;
    // }

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glUseProgram(program);

    model_matrix_loc = glGetUniformLocation(program, "model_matrix");
    projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");

    std::cout << "model_matrix_loc, projection_matrix_loc: " << model_matrix_loc << ", " << projection_matrix_loc << std::endl;

    object.loadFromVBM("../src/03-drawing_with_opengl/armadillo_low.vbm", 0, 1, 2);
    object.bindVertexArray();

    // Generate the colors of the objects
    vmath::vec4 colors[INSTANCE_COUNT];

    for (int n = 0; n < INSTANCE_COUNT; n++)
    {
        float a = float(n) / 4.0f;
        float b = float(n) / 5.0f;
        float c = float(n) / 6.0f;

        colors[n][0] = 0.5f * (sinf(a + 1.0f) + 1.0f);
        colors[n][1] = 0.5f * (sinf(b + 2.0f) + 1.0f);
        colors[n][2] = 0.5f * (sinf(c + 3.0f) + 1.0f);
        colors[n][3] = 1.0f;
    }

    // Create and allocate the VBO to hold the weights
    // Notice that we use the 'colors' array as the initial data, but only because
    // we know it's the same size.
    glGenBuffers(1, &weight_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, weight_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);

    glVertexAttribDivisor(3, 1);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(3);

    // Same with the instance color array
    glGenBuffers(1, &color_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glVertexAttribDivisor(4, 1);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

void display() {
    std::time_t curtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    float t = float(curtime % 60) / float(60);

    // std::cout << "curtime:" << curtime << " t:" << t <<std::endl;

    static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
    static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
    static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

    // t = 0.3f;

    vmath::vec4 weights[INSTANCE_COUNT];

    for (int i = 0; i < INSTANCE_COUNT; ++i) {
        float a = float(i) / 4.0f;
        float b = float(i) / 5.0f;
        float c = float(i) / 6.0f;

        weights[i][0] = 0.5f * (sinf(t * 6.28318531f * 8.0f + a) + 1.0f);
        weights[i][1] = 0.5f * (sinf(t * 6.28318531f * 26.0f + b) + 1.0f);
        weights[i][2] = 0.5f * (sinf(t * 6.28318531f * 21.0f + c) + 1.0f);
        weights[i][3] = 0.5f * (sinf(t * 6.28318531f * 13.0f + a + b) + 1.0f);
    }

    glBindBuffer(GL_ARRAY_BUFFER, weight_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(weights), weights, GL_DYNAMIC_DRAW);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Activate instancing program
    glUseProgram(program);

    // Set four model matrices
    vmath::mat4 model_matrix[4];

    for (int n = 0; n < 4; n++)
    {
        model_matrix[n] = (vmath::scale(5.0f) *
                           vmath::rotate(t * 360.0f * 40.0f + float(n + 1) * 29.0f, 0.0f, 1.0f, 0.0f) *
                           vmath::rotate(t * 360.0f * 20.0f + float(n + 1) * 35.0f, 0.0f, 0.0f, 1.0f) *
                           vmath::rotate(t * 360.0f * 30.0f + float(n + 1) * 67.0f, 0.0f, 1.0f, 0.0f) *
                           vmath::translate((float)n * 10.0f - 15.0f, 0.0f, 0.0f) *
                           vmath::scale(0.01f));
    }

    glUniformMatrix4fv(model_matrix_loc, 4, GL_FALSE, model_matrix[0]);

    // Set up the projection matrix
    vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f) * vmath::translate(0.0f, 0.0f, -50.0f));

    glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix);

    object.render(0, INSTANCE_COUNT);
}

void finalize() {
    glUseProgram(0);
    glDeleteProgram(program);
    glDeleteBuffers(1, &weight_vbo);
    glDeleteBuffers(1, &color_vbo);
}

int main() {
    glfwSetErrorCallback(error_callback);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "instancing", NULL, NULL);
    glfwSetWindowSizeCallback(window, size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress));

    glViewport(0, 0, 800, 600);
    aspect = 600.0f / 800.0f;

    init();

    while(!glfwWindowShouldClose(window)) {
        display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    finalize();

    glfwDestroyWindow(window);
    glfwTerminate();
}
