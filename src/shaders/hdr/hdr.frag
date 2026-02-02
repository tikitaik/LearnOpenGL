# version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D tex;

void main() {

    const float exposure = 1.0f;
    const float gamma = 2.2f;

    vec3 hdrColor = texture(tex, TexCoords).rgb;

    vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0f / gamma));

    FragColor = vec4(mapped, 1.0f);
}
