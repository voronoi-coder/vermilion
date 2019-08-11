vec3 velocity = vec3(0.0, 2.0, 3.0);

ivec3 steps = ivec3(velocity);

vec4 color;
vec3 RGB = vec3(color);

vec3 white = vec3(1.0);
vec4 translucent = vec4(white, 0.5);

| 1.0 | 4.0 | 7.0 |
| 2.0 | 5.0 | 8.0 |
| 3.0 | 6.0 | 9.0 |

mat3 M = mat3(1.0,2.0,3.0,
              4.0,5.0,6.0,
              7.0,8.0,9.0);

vec3 column1 = vec3(1.0, 2.0, 3.0);
vec3 column2 = vec3(4.0, 5.0, 6.0);
vec3 column3 = vec3(7.0, 8.0, 9.0);

mat3 M = mat3(column1, column2, column3);
mat3 M = mat3(column1, column2, column3);

float red = color.r;
float v_y = velocity.y;

float red = color[0];
float v_y = velocity[1];

vec3 luminance = color.rrr;
color = color.abgr;

mat4 m = mat4(2.0);
vec4 zVec = m[2];
float yScale = m[1][1];

struct Particle {
    float lifetime;
    vec3 position;
    vec3 velocity;
}

layout(std140) uniform b {
    float size;
    layout(offset=32) vec4 color;
    layout(align=1024) vec4 a[12];
    vec4 b[12];
} buf;
Particle p = Particle(10.0, pos, vel);

float coeff[3];
float[3] coeff;
int indices[];

float coeff[3] = float(1.0, 1.0, 1.0);
coeff.length();

mat3x4 m;
m.length() == 3;
m[0].length() = 4;
vec3 v;
mat3 m;
vec3 result = v * m;

vec2 c, a, b;
mat2 m, u, v;

c = a * b;
m = u * v;

invariant gl_Position;


buffer BufferObject {
    int mode;
    vec4 points[];
}

in Lighting {
    vec3 normal;
    vec2 bumpCoord;
}

out Lighting {
    vec3 normal;
    vec2 bumpCoord;
}

#version 440
in Lighting {
    layout(location=1, component=0) vec2 offset;
    layout(location=1, component=2) vec2 bumpCoord;
}

subroutine vec4 LightFunc(vec3);

subroutine (LightFunc) vec4 ambient(vec3 n) {
    return Materials.ambient;
}
subroutine (LightFunc) vec4 diffuse(vec3 n) {
    return Materials.diffuse;
}

subroutine uniform LightFunc materialShader;
