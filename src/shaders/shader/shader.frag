#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;

vec3 BlinnPhong();
float ShadowCalculation(vec3 fragPos);
vec4 MandelbrotSet(vec4 fragCoord);

void main() {
    FragColor = vec4(BlinnPhong(), 1.0f);
}

float ShadowCalculation(vec3 fragPos) {

    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(depthMap, fragToLight).r;

    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);

    float shadow  = 0.0;
    float bias    = 0.05; 
    float samples = 4.0;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0f + (viewDistance / far_plane)) / 25.0f;

    vec3 sampleOffsetDirections[20] = vec3[] (
         vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
         vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
         vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
         vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
         vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    for(int i = 0; i < 20; i++) {

        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r; 
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples);

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
    float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);
    float shadow = ShadowCalculation(fs_in.FragPos);
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
