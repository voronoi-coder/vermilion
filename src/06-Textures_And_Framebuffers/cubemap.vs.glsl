#version 410

layout (location = 0) in vec3 in_position;

out vec3 tex_coord;

uniform mat4 tc_rotate;

void main() {
    gl_Position = tc_rotate * vec4(in_position, 1.0);
    tex_coord = in_position;
}
