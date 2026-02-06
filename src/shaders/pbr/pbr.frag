# version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform vec3 camPos;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

vec3 fresnelShlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

const float PI = 3.14159265359;

void main() {

    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0f);

    for (int i = 0; i < 4; i++) {

        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L); // halfway vector

        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0f / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelShlick(max(dot(H, V), 0.0f), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0f - kS);

        kD *= 1.0f - metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
        vec3 specular = numerator / denominator;

        // add outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0f);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03f) * albedo * ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0f));
    color = pow(color, vec3(1.0f/2.2f));

    FragColor = vec4(color, 1.0f);
}

vec3 fresnelShlick(float cosTheta, vec3 F0) {

    return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {

    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.f);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {

    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float num = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return num / denom;

}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {

    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;

}
