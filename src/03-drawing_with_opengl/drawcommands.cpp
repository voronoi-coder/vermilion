#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>

#include <vmath.h>

static void error_callback(int code, const char *description) {
    std::cerr <<"Error: " << description <<std::endl;
}

static const GLchar *readShader(const char * filename){
    FILE *infile = fopen(filename, "rb");
    if(!infile) {
        std::cerr << "Unable to open file:" << filename <<"; errno:" << errno <<std::endl;
        return NULL;
    }
    fseek(infile, 0, SEEK_END);
    int len = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    GLchar *source = new GLchar[len+1];
    fread(source, 1, len, infile);
    fclose(infile);
    source[len] = 0;

    return source;
}

float aspect;
GLuint vbo[1];
GLuint vao[1];
GLuint ebo[1];
GLint model_matrix_loc;
GLint projection_matrix_loc;

int main() {
    glfwSetErrorCallback(error_callback);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "draw commands", NULL, NULL);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, 800, 600);
    aspect = 600.0f / 800.f;

    // init
    const GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    auto vs_src = readShader("../src/03-drawing_with_opengl/vs.glsl");
    auto fs_src = readShader("../src/03-drawing_with_opengl/fs.glsl");
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
    
    const GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glUseProgram(program);

    model_matrix_loc = glGetUniformLocation(program, "model_matrix");
    projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");

    // A single triangle
    static const GLfloat vertex_position[] = {
        -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  0.0f, 1.0f,
    };
    // Color for each vertex
    static const GLfloat vertex_colors[] =
    {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f
    };
    // Indices for the triangle strips
    static const GLushort vertex_indices[] =
    {
        0, 1, 2
    };

    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_position) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_position), vertex_position);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_position), sizeof(vertex_colors), vertex_colors);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_position));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    while(!glfwWindowShouldClose(window)) {
        static float q = 0.0f;
        static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
        static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
        static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);
        static const vmath::vec4 black = vmath::vec4(0.0f, 0.0f, 0.0f, 0.0f);

        vmath::mat4 model_matrix;

        // glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        glClearBufferfv(GL_COLOR, 0, black);

        // Activate simple shading program
        glUseProgram(program);

        // Set up the model and projection matrix
        vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.0f));
        glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix);

        // Set up for a glDrawElements call
        glBindVertexArray(vao[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);

        // Draw Arrays...
        model_matrix = vmath::translate(-3.0f, 0.0f, -5.0f);
        glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, model_matrix);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        model_matrix = vmath::translate(-1.0f, 0.0f, -5.0f);
        glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, model_matrix);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

        // DrawElementsBaseVertex
        model_matrix = vmath::translate(1.0f, 0.0f, -5.0f);
        glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, model_matrix);
        glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);

        model_matrix = vmath::translate(3.0f, 0.0f, -5.0f);
        glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, model_matrix);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glUseProgram(0);
    glDeleteProgram(program);
    glDeleteVertexArrays(1, vao);
    glDeleteBuffers(1, vbo);

    glfwDestroyWindow(window);
    glfwTerminate();
}
