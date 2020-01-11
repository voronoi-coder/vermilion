#version 410

in vec2 tex_coord;

layout (location = 0) out vec4 color;

uniform sampler2D tex;

void main() {
    color = texture(tex, tex_coord);
}
