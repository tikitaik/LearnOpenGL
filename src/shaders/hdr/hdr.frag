# version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D tex;

void main() {

    const float exposure = 1.0f;

    vec3 hdrColor = texture(tex, TexCoords).rgb;

    vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure);

    FragColor = vec4(mapped, 1.0f);
}
