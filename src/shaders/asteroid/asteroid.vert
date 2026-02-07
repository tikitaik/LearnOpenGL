#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

out vec3 FragPos;
out vec3 Normal;
out vec2 texCoords;

void main() {
    FragPos = aPos;
    Normal = aNormal;
    texCoords = aTexCoords;
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0f);
}
