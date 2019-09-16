#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <targa.h>
#include <vmath.h>

#include <iostream>

using namespace vmath;

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

#define POINT_COUNT 4

float aspect;
GLuint sprite_texture;
GLuint program;
GLuint vao[0];
GLuint vbo[0];

GLint model_matrix_loc;
GLint projection_matrix_loc;

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

static inline float random_float()
{
    float res;
    unsigned int tmp;
    static unsigned int seed = 0x13371337;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);

    *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

    return (res - 1.0f);
}

void init() {
    GLenum format;
    int width, height;
    unsigned char *data;

    // glGenTextures(1, &sprite_texture);
    // glBindTexture(GL_TEXTURE_2D, sprite_texture);

    // data = vtarga::load_targa("../src/03-drawing_with_opengl/sprite.tga", format, width, height);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // delete [] data;
    // glGenerateMipmap(GL_TEXTURE_2D);

    const GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *vs_src = readShader("./src/03-drawing_with_opengl/pointsprites.vs.glsl");
    const GLchar *fs_src = readShader("./src/03-drawing_with_opengl/pointsprites.fs.glsl");
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
    // A single triangle
    static vec4 * vertex_positions;

    // Set up the vertex attributes
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, POINT_COUNT * sizeof(vec4), NULL, GL_STATIC_DRAW);

    vertex_positions = (vec4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (int n = 0; n < POINT_COUNT; n++)
    {
        vertex_positions[n] = vec4(random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f, 1.0f);
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_positions));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
void display() {
    std::time_t curtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    float t = float(curtime % 60) / float(60);

    static float q = 0.0f;
    static const vec3 X(1.0f, 0.0f, 0.0f);
    static const vec3 Y(0.0f, 1.0f, 0.0f);
    static const vec3 Z(0.0f, 0.0f, 1.0f);

    mat4 model_matrix;

    // Setup
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate simple shading program
    glUseProgram(program);

    // Set up the model and projection matrix
    mat4 projection_matrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 8.0f));
    glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix);

    // glBindTexture(GL_TEXTURE_2D, sprite_texture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(32.0f);

    // Draw Arrays...
    model_matrix = translate(0.0f, 0.0f, -2.0f) *
                   rotate(t * 360.0f, Y) * rotate(t * 720.0f, Z);
    glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, model_matrix);
    glDrawArrays(GL_POINTS, 0, POINT_COUNT);    
}
void finalize() {
    glUseProgram(0);
    glDeleteVertexArrays(1, vao);
    glDeleteBuffers(1, vbo);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(600, 600, "pointsprites", NULL, NULL);

    glfwSetWindowSizeCallback(window, size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, 600, 600);
    aspect = 600.0f / 600.0f;

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
