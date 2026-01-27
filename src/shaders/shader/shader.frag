#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 BlinnPhong();
float ShadowCalculation(vec4 fragPosLightSpace);
vec4 MandelbrotSet(vec4 fragCoord);

void main() {
    FragColor = vec4(BlinnPhong(), 1.0f);
}

float ShadowCalculation(vec4 fragPosLightSpace) {

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float shadow = currentDepth > closestDepth ? 1.0f : 0.0f;

    return shadow;
}

vec3 BlinnPhong() {

    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0f);

    // ambient
    vec3 ambient = 0.15f * lightColor;

    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float spec = 0.0f;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * lightColor;

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * color;

    return lighting;
}

vec4 MandelbrotSet(vec4 fragCoord) {

    float ca = (fragCoord.x - 400) / 200.0f;
    float cb = (fragCoord.y - 300) / 200.0f;

    float za = 0;
    float zb = 0;

    float max = 40;
    float i = 0;

    while (za * za + zb * zb < 50 && i < max) {

        float temp = za;
        temp = za * za - zb * zb;
        zb = 2 * za * zb;
        za = temp;

        za += ca;
        zb += cb;

        i++;
    }

    float ratio = i / max;

    return vec4(ratio, ratio, ratio, 1.0f);
}
