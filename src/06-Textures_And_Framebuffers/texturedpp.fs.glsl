#version 410

uniform sampler2D sprite_texture;

out vec4 color;

void main() {
    color = texture(sprite_texture, vec2(gl_PointCoord.x, 1 - gl_PointCoord.y));
}
