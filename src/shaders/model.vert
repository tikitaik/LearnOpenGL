#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout (std140, binding = 0) uniform Matrices {
    mat4 projection;
    mat4 view;
};

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;

void main() {
    FragPos = aPos;
    Normal = aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
