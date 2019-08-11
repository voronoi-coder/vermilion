static const char *source = R"SHADER(

=================Declaring Variables=================
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

Particle p = Particle(10.0, pos, vel);

float coeff[3];
float[3] coeff;
int indices[];

float coeff[3] = float(1.0, 1.0, 1.0);
coeff.length();

mat3x4 m;
m.length() == 3;
m[0].length() = 4;

======================Storage Qualifiers=================
// const in out uniform buffer shared
const float Pi = 3.141529;

GLint timeLoc;
GLfloat timeValue;

timeLoc = glUniformLocation(program, "time");
glUniform1f(timeLoc, timeValue);

======================Statements=================
vec3 v;
mat3 m;
vec3 result = v * m;

vec2 c, a, b;
mat2 m, u, v;

c = a * b;
m = u * v;
discard;

function:   in  const in    out     inout

invariant gl_Position;

precise gl_Position;
precise out vec3 Location;
precise vec3 subdivide(vec3 P1, vec3 P2) {}

======================Interface Blocks=================
uniform Matrices {
    mat4 ModelView;
    mat4 Projection;
    mat4 Color;
};

layout(shared, row_major) uniform {};
layout(packed, column_major) uniform {};

layout(std140) uniform b {
    float size;
    layout(offset=32) vec4 color;
    layout(align=1024) vec4 a[12];
    vec4 b[12];
} buf;

)SHADER";
