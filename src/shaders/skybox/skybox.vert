# version 330 core

layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

out vec3 localPos;

void main() {
    
    localPos = aPos;
    mat4 rotView = mat4(mat3(view));
    vec4 pos = projection * rotView * vec4(aPos, 1.0f);
    gl_Position = pos.xyww;
}
