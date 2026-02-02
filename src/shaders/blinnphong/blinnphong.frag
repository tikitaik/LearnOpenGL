#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 LightPos;
    vec3 ViewPos;
} fs_in;

uniform sampler2D diffuseMap;

uniform float far_plane;

vec3 BlinnPhong();

void main() {
    FragColor = vec4(BlinnPhong(), 1.0f);
}

vec3 BlinnPhong() {

    vec3 normal = fs_in.Normal;
    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;

    // ambient
    vec3 ambient = 0.1f * color;

    // diffuse
    vec3 lightDir = normalize(fs_in.LightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(fs_in.ViewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);
    vec3 specular = vec3(0.2f) * spec;

    // calculate shadow
    float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);

    vec3 lighting = ambient + diffuse + specular;
    return lighting;
}
