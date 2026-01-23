#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140, binding = 0) uniform Matrices {
    mat4 projection;
    mat4 view;
};

out vec3 FragPos;
out vec3 Normal;

out VS_OUT {
    vec2 texCoords;
} vs_out;

uniform mat4 model;

void main() {
    FragPos = aPos;
    Normal = aNormal;
    vs_out.texCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
