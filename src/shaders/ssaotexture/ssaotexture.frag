# version 330 core

out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

const vec2 noiseScale = vec2(1280.0/4.0, 720.0/4.0);

void main() {

    vec3 fragPos   = texture(gPosition, TexCoords).xyz;
    vec3 normal    = texture(gNormal, TexCoords).rgb;
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);  
    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    float occlusion = 0.0f;

    const int kernelSize = 64;
    const float radius = 0.5f;
    const float bias = 0.025f;

    for (int i = 0; i < kernelSize; i++) {

        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * radius;

        vec4 offset = vec4(samplePos, 1.0);
        offset      = projection * offset;    // from view to clip-space
        offset.xyz /= offset.w;               // perspective divide
        offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        float sampleDepth = texture(gPosition, offset.xy).z;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion       += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0);
    }

    occlusion = 1.0f - (occlusion / kernelSize);
    FragColor = occlusion;
}
