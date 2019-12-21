#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint  VAOs[NumVAOs];
GLuint  Buffers[NumBuffers];

const GLuint  NumVertices = 6;

static const char *vertex_shader_text = R"SHADER(
#version 410 core

layout( location = 0 ) in vec4 vPosition;

void main() {
    gl_Position = vPosition;
}
)SHADER";

static const char *frag_shader_text = R"SHADER(
#version 410 core

out vec4 fColor;

void main()
{
    fColor = vec4(0.5, 0.4, 0.8, 1.0);
}
)SHADER";

static void error_callback(int code, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

void init() {
    glGenVertexArrays( NumVAOs, VAOs );
    glBindVertexArray( VAOs[Triangles] );

    GLfloat  vertices[NumVertices][2] = {
        { -0.90f, -0.90f }, {  0.85f, -0.90f }, { -0.90f,  0.85f },  // Triangle 1
        {  0.90f, -0.85f }, {  0.90f,  0.90f }, { -0.85f,  0.90f }   // Triangle 2
    };

    glGenBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &frag_shader_text, NULL);
    glCompileShader(fragment_shader);

    // GLint compiled;
    // glGetShaderiv( fragment_shader, GL_COMPILE_STATUS, &compiled );
    // if ( !compiled ) {
    //     GLsizei len;
    //     glGetShaderiv( fragment_shader, GL_INFO_LOG_LENGTH, &len );
    //
    //     GLchar* log = new GLchar[len+1];
    //     glGetShaderInfoLog( fragment_shader, len, &len, log );
    //     std::cerr << "Shader compilation failed: " << log << std::endl;
    //     delete [] log;
    // }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // GLint linked;
    // glGetProgramiv(program, GL_LINK_STATUS, &linked);
    // if ( !linked ) {
    //     GLsizei len;
    //     glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    //
    //     GLchar* log = new GLchar[len+1];
    //     glGetProgramInfoLog(program, len, &len, log);
    //     std::cerr << "Shader linking failed: " << log << std::endl;
    //     delete [] log;
    //
    //     glDeleteShader(vertex_shader);
    //     glDeleteShader(fragment_shader);
    // }

    glUseProgram(program);

    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(vPosition);
}

void display()
{
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearBufferfv(GL_COLOR, 0, black);

    glBindVertexArray(VAOs[Triangles]);
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

int main(int argc, char** argv) {
    glfwSetErrorCallback(error_callback);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Triangles", NULL, NULL);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // const GLubyte * OpenGLVersion = glGetString(GL_VERSION);
    // printf("opengl version %s\n", OpenGLVersion);

    // not support opengl 4.5
    init();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glad_glPointSize(64);

    while(!glfwWindowShouldClose(window)) {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
