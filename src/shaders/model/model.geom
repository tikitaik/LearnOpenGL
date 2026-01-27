# version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
    vec3 fragPos;
    vec3 normal;
} gs_in[];

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform float time;

vec4 explode(vec4 position, vec3 normal);
vec3 GetNormal();

void main() {
    
    Normal = GetNormal();

    gl_Position = gl_in[0].gl_Position;
    TexCoords = gs_in[0].texCoords;
    FragPos = gs_in[0].fragPos;
    Normal = gs_in[0].normal;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    TexCoords = gs_in[1].texCoords;
    FragPos = gs_in[1].fragPos;
    Normal = gs_in[1].normal;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    TexCoords = gs_in[2].texCoords;
    FragPos = gs_in[2].fragPos;
    Normal = gs_in[2].normal;
    EmitVertex();
    EndPrimitive();
}

vec4 explode(vec4 position, vec3 normal) {

    float magnitude = 2.0f;
    vec3 direction = normal * ((sin(time) + 1.0f) / 2.0f) * magnitude;
    return position + vec4(direction, 0.0f);
}

vec3 GetNormal() {
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}
