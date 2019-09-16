#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <vbm.h>
#include <vmath.h>

using namespace vmath;

#define INSTANCE_COUNT 200
float aspect;

GLuint color_buffer;
GLuint model_matrix_buffer;
GLuint color_tbo;
GLuint model_matrix_tbo;
GLuint program;

GLint view_matrix_loc;
GLint projection_matrix_loc;

VBObject object;

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

    const GLchar *vs_src = readShader("../src/03-drawing_with_opengl/instancing3.vs.glsl");
    const GLchar *fs_src = readShader("../src/03-drawing_with_opengl/instancing3.fs.glsl");
    glShaderSource(vs, 1, &vs_src, NULL);
    glShaderSource(fs, 1, &fs_src, NULL);
    glCompileShader(vs);
    glCompileShader(fs);

    // GLint compiled;
    // glGetShaderiv( fs, GL_COMPILE_STATUS, &compiled );
    // if ( !compiled ) {
    //     GLsizei len;
    //     glGetShaderiv( fs, GL_INFO_LOG_LENGTH, &len );
    //
    //     GLchar* log = new GLchar[len+1];
    //     glGetShaderInfoLog( fs, len, &len, log );
    //     std::cerr << "Shader compilation failed: " << log << std::endl;
    //     delete [] log;
    // }

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glUseProgram(program);

    view_matrix_loc = glGetUniformLocation(program, "view_matrix");
    projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");

    std::cout << "view_matrix_loc, projection_matrix_loc: " << view_matrix_loc << ", " << projection_matrix_loc << std::endl;

    GLuint color_tbo_loc = glGetUniformLocation(program, "color_tbo");
    GLuint model_matrix_tbo_loc = glGetUniformLocation(program, "model_matrix_tbo");

    glUniform1i(color_tbo_loc, 0);
    glUniform1i(model_matrix_tbo_loc, 1);

    object.loadFromVBM("../src/03-drawing_with_opengl/armadillo_low.vbm", 0, 1, 2);

    glGenTextures(1, &color_tbo);
    glBindTexture(GL_TEXTURE_BUFFER, color_tbo);
    vec4 colors[INSTANCE_COUNT];
    for (int i = 0; i < INSTANCE_COUNT; ++i) {
        float a = float(i) / 4.0f;
        float b = float(i) / 5.0f;
        float c = float(i) / 6.0f;

        colors[i][0] = 0.5f + 0.25f * (sinf(a + 1.0f) + 1.0f);
        colors[i][1] = 0.5f + 0.25f * (sinf(b + 2.0f) + 1.0f);
        colors[i][2] = 0.5f + 0.25f * (sinf(c + 3.0f) + 1.0f);
        colors[i][3] = 1.0f;
    }
    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_TEXTURE_BUFFER, color_buffer);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, color_buffer);

    glGenTextures(1, &model_matrix_tbo);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, model_matrix_tbo);
    glGenBuffers(1, &model_matrix_buffer);
    glBindBuffer(GL_TEXTURE_BUFFER, model_matrix_buffer);
    glBufferData(GL_TEXTURE_BUFFER, INSTANCE_COUNT * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, model_matrix_buffer);
    // glActiveTexture(GL_TEXTURE0);
}

void display() {
    std::time_t curtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    float t = float(curtime % 60) / float(60);

    // std::cout << "curtime:" << curtime << " t:" << t <<std::endl;

    static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
    static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
    static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

    int n;

    // Set model matrices for each instance
    mat4 matrices[INSTANCE_COUNT];

    for (n = 0; n < INSTANCE_COUNT; n++)
    {
        float a = 50.0f * float(n) / 4.0f;
        float b = 50.0f * float(n) / 5.0f;
        float c = 50.0f * float(n) / 6.0f;

        matrices[n] = vmath::rotate(a + t * 360.0f, 1.0f, 0.0f, 0.0f) *
            vmath::rotate(b + t * 360.0f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate(c + t * 360.0f, 0.0f, 0.0f, 1.0f) *
            vmath::translate(10.0f + a, 40.0f + b, 50.0f + c);
    }

    glBindBuffer(GL_TEXTURE_BUFFER, model_matrix_buffer);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(matrices), matrices, GL_DYNAMIC_DRAW);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Activate instancing program
    glUseProgram(program);

    // Set up the view and projection matrices
    mat4 view_matrix(vmath::translate(0.0f, 0.0f, -1500.0f) * vmath::rotate(t * 360.0f * 2.0f, 0.0f, 1.0f, 0.0f));
    mat4 projection_matrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f));

    glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, view_matrix);
    glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix);

    // Render INSTANCE_COUNT objects
    object.render(0, INSTANCE_COUNT);
}

void finalize() {
    glUseProgram(0);
    glDeleteProgram(program);
    glDeleteBuffers(1, &color_buffer);
    glDeleteBuffers(1, &model_matrix_buffer);
}

int main() {
    glfwSetErrorCallback(error_callback);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "instancing3", NULL, NULL);

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
