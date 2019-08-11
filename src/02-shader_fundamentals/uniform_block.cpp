#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

void error_callback(int code, const char *description) {
    std::cerr << "Error: " << description << std::endl;
}

size_t TypeSize(GLenum type) {
    size_t size;

    #define CASE(Enum, Count, Type) \
    case Enum: size = Count * sizeof(Type); break

    switch(type) {
        CASE(GL_FLOAT,				 1,				 GLfloat);
        CASE(GL_FLOAT_VEC2,				2,				 GLfloat);
        CASE(GL_FLOAT_VEC3,				3,				 GLfloat);
        CASE(GL_FLOAT_VEC4,				4,				 GLfloat);
        CASE(GL_INT,				1,				 GLint);
        CASE(GL_INT_VEC2,				2,				 GLint);
        CASE(GL_INT_VEC3,				3,				 GLint);
        CASE(GL_INT_VEC4,				4,				 GLint);
        CASE(GL_UNSIGNED_INT,				1,				 GLuint);
        CASE(GL_UNSIGNED_INT_VEC2,				 2,				 GLuint);
        CASE(GL_UNSIGNED_INT_VEC3,				 3,				 GLuint);
        CASE(GL_UNSIGNED_INT_VEC4,				 4,				 GLuint);
        CASE(GL_BOOL,				1,				 GLboolean);
        CASE(GL_BOOL_VEC2,				2,				 GLboolean);
        CASE(GL_BOOL_VEC3,				3,				 GLboolean);
        CASE(GL_BOOL_VEC4,				4,				 GLboolean);
        CASE(GL_FLOAT_MAT2,				4,				 GLfloat);
        CASE(GL_FLOAT_MAT2x3,				6,				 GLfloat);
        CASE(GL_FLOAT_MAT2x4,				8,				 GLfloat);
        CASE(GL_FLOAT_MAT3,				9,				 GLfloat);
        CASE(GL_FLOAT_MAT3x2,				6,				 GLfloat);
        CASE(GL_FLOAT_MAT3x4,				12,				 GLfloat);
        CASE(GL_FLOAT_MAT4,				16,				 GLfloat);
        CASE(GL_FLOAT_MAT4x2,				8,				 GLfloat);
        CASE(GL_FLOAT_MAT4x3,				12,				 GLfloat);
        #undef CASE

        default:
            fprintf(stderr, "Unknown type:Ox%x\n", type);
            exit(EXIT_FAILURE);
            break;
    }

    return size;
}

void init() {
    GLuint program;

    glClearColor(1, 0, 0, 1);

    // program = LoadShader();
    glUseProgram(program);

    GLuint uboIndex;
    GLint uboSize;
    GLuint ubo;
    GLbyte *buffer;

    uboIndex = glGetUniformBlockIndex(program, "Uniforms");
    glGetActiveUniformBlockiv(program, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);

    buffer = (GLbyte *)malloc(uboSize);

    if(buffer == NULL) {
        fprintf(stderr, "Unable to allocate buffer\n");
        exit(EXIT_FAILURE);
    } else {
        enum {Translation, Scale, Rotation, Enabled, NumUniforms};

        GLfloat scale = 0.5;
        GLfloat translation[] = {0.1, 0.1, 0.0};
        GLfloat rotation[] = {90, 0.0, 0.0, 1.0};
        GLboolean enabled = GL_TRUE;

        const char *names[NumUniforms] = {"translation", "scale", "rotation", "enabled"};

        GLuint indices[NumUniforms];
        GLint size[NumUniforms];
        GLint offset[NumUniforms];
        GLint type[NumUniforms];

        glGetUniformIndices(program, NumUniforms, names, indices);
        glGetActiveUniformsiv(program, NumUniforms, indices, GL_UNIFORM_OFFSET, offset);
        glGetActiveUniformsiv(program, NumUniforms, indices, GL_UNIFORM_SIZE, size);
        glGetActiveUniformsiv(program, NumUniforms, indices, GL_UNIFORM_TYPE, type);

        memcpy(buffer + offset[Scale], &scale, size[Scale] * TypeSize(type[Scale]));
        memcpy(buffer + offset[Translation], &translation, size[Translation] * TypeSize(type[Translation]));
        memcpy(buffer + offset[Rotation], &rotation, size[Rotation] * TypeSize(type[Rotation]));
        memcpy(buffer + offset[Enabled], &enabled, size[Enabled] * TypeSize(type[Enabled]));

        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, uboSize, buffer, GL_STATIC_DRAW);

        glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex, ubo);

        GLint materialShaderLoc;
        GLuint ambientIndex;
        GLuint diffuseIndex;

        materialShaderLoc = glGetSubroutineUniformLocation(program, GL_VERTEX_SHADER, "materialShade");

        ambientIndex = glGetSubroutineIndex(program, GL_VERTEX_SHADER, "ambient");
        diffuseIndex = glGetSubroutineIndex(program, GL_VERTEX_SHADER, "diffuse");

        GLsizei n;
        glad_glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &n);

        GLuint *indices2 = new GLuint[n];
        indices[materialShaderLoc] = ambientIndex;

        glad_glUniformSubroutinesuiv(GL_VERTEX_SHADER, n, indices2);

        delete[] indices2;
    }
}

int main() {
    glfwSetErrorCallback(error_callback);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "uniform block", NULL, NULL);
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
