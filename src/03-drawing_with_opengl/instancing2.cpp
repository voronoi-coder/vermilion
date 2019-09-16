#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <vbm.h>
#include <vmath.h>

using namespace vmath;

#define INSTANCE_COUNT 200

float aspect;
GLuint program;
GLuint weight_vbo;
GLuint color_vbo;

GLuint color_buffer;
GLuint model_matrix_buffer;
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

    const GLchar *vs_src = readShader("../src/03-drawing_with_opengl/instancing2.vs.glsl");
    const GLchar *fs_src = readShader("../src/03-drawing_with_opengl/instancing2.fs.glsl");
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

    object.loadFromVBM("../src/03-drawing_with_opengl/armadillo_low.vbm", 0, 1, 2);
    object.bindVertexArray();

    int color_loc       = glGetAttribLocation(program, "color");
    int matrix_loc      = glGetAttribLocation(program, "model_matrix");
    std::cout << "view_matrix_loc, projection_matrix_loc: " << view_matrix_loc << ", " << projection_matrix_loc << std::endl;

    // Generate the colors of the objects
    vec4 colors[INSTANCE_COUNT];

    int n = 0;
    for (n = 0; n < INSTANCE_COUNT; n++)
    {
        float a = float(n) / 4.0f;
        float b = float(n) / 5.0f;
        float c = float(n) / 6.0f;

        colors[n][0] = 0.5f + 0.25f * (sinf(a + 1.0f) + 1.0f);
        colors[n][1] = 0.5f + 0.25f * (sinf(b + 2.0f) + 1.0f);
        colors[n][2] = 0.5f + 0.25f * (sinf(c + 3.0f) + 1.0f);
        colors[n][3] = 1.0f;
    }

    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);

    // Now we set up the color array. We want each instance of our geometry
    // to assume a different color, so we'll just pack colors into a buffer
    // object and make an instanced vertex attribute out of it.
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(color_loc);
    // This is the important bit... set the divisor for the color array to
    // 1 to get OpenGL to give us a new value of 'color' per-instance
    // rather than per-vertex.
    glVertexAttribDivisor(color_loc, 1);

    // Likewise, we can do the same with the model matrix. Note that a
    // matrix input to the vertex shader consumes N consecutive input
    // locations, where N is the number of columns in the matrix. So...
    // we have four vertex attributes to set up.
    glGenBuffers(1, &model_matrix_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, model_matrix_buffer);
    glBufferData(GL_ARRAY_BUFFER, INSTANCE_COUNT * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);
    // Loop over each column of the matrix...
    for (int i = 0; i < 4; i++)
    {
        // Set up the vertex attribute
        glVertexAttribPointer(matrix_loc + i,              // Location
                4, GL_FLOAT, GL_FALSE,       // vec4
                sizeof(mat4),                // Stride
                (void *)(sizeof(vec4) * i)); // Start offset
        // Enable it
        glEnableVertexAttribArray(matrix_loc + i);
        // Make it instanced
        glVertexAttribDivisor(matrix_loc + i, 1);
    }

    // Done (unbind the object's VAO)
    glBindVertexArray(0);

}

void display() {
    std::time_t curtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    float t = float(curtime % 60) / float(60);

    // std::cout << "curtime:" << curtime << " t:" << t <<std::endl;

    static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
    static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
    static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindBuffer(GL_ARRAY_BUFFER, model_matrix_buffer);
    mat4 *matrices = (mat4*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for(int i = 0; i < INSTANCE_COUNT; ++i) {
        float a = 50.0f * float(i) / 4.0f;
        float b = 50.0f * float(i) / 5.0f;
        float c = 50.0f * float(i) / 6.0f;

        matrices[i] = rotate(a + t * 360.0f, 1.0f, 0.0f, 0.0f) *
                      rotate(b + t * 360.0f, 0.0f, 1.0f, 0.0f) *
                      rotate(c + t * 360.0f, 0.0f, 0.0f, 1.0f) *
                      translate(10.0f + a, 40.0f + b, 50.0f + c);

    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Activate instancing program
    glUseProgram(program);

    // Set up the view and projection matrices
    mat4 view_matrix(translate(0.0f, 0.0f, -1500.0f) * rotate(t * 360.0f * 2.0f, 0.0f, 1.0f, 0.0f));
    mat4 projection_matrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f));

    glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, view_matrix);
    glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix);

        // Render INSTANCE_COUNT objects
    object.render(0, INSTANCE_COUNT);

    lookat(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
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

    GLFWwindow *window = glfwCreateWindow(800, 600, "instancing2", NULL, NULL);

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
