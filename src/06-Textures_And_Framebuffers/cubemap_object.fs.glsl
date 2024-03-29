#version 410

in vec3 vs_fs_normal;
in vec3 vs_fs_position;

layout (location = 0) out vec4 color;

uniform samplerCube tex;

void main() {
    vec3 tc = reflect(vs_fs_position, normalize(vs_fs_normal));
    color = vec4(0.3, 0.2, 0.2, 1.0) + vec4(0.97, 0.83, 0.79, 0.0) * texture(tex, tc);
    /* color = texture(tex, tc); */
}
