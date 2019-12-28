#version 410

in vec2 tex_coord0;
in vec2 tex_coord1;

out vec4 color;

uniform sampler2D tex1;
uniform sampler2D  tex2;

void main() {
    color = texture(tex1, tex_coord0) + texture(tex2, tex_coord1);
}
