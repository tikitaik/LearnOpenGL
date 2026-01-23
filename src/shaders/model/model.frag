#version 330 core

#define NR_POINT_LIGHTS 4

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 pos;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float innerCutoff;
    float outerCutoff;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform sampler2D ourTex;

//uniform PointLight pointLights[NR_POINT_LIGHTS];
//uniform SpotLight spotLight;

out vec4 FragColor;  

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
  
void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0f);

    result += CalcDirLight(dirLight, norm, viewDir);

    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
         //result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {

    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(lightDir, normal), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    vec3 ambient = light.ambient *
        vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * 
        vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec *
        vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {

    vec3 lightDir = normalize(light.pos - fragPos);

    float distance = length(light.pos - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + 
        light.quadratic * distance * distance);

    float diff = max(dot(lightDir, normal), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    vec3 ambient = light.ambient *
        vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * 
        vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec *
        vec3(texture(material.specular, TexCoords));

    return attenuation * (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {

    vec3 lightDir = normalize(light.direction);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    float cosTheta = dot(lightDir, normalize(-viewDir));

    // check for if in spotlight
    if (cosTheta > light.outerCutoff) {

        float diff = max(dot(-lightDir, normal), 0.0);

        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

        diffuse = light.diffuse * diff * 
            vec3(texture(material.diffuse, TexCoords));
        specular = light.specular * spec *
            vec3(texture(material.specular, TexCoords));
    }
    if (cosTheta < light.innerCutoff) {

        float epsilon = light.innerCutoff - light.outerCutoff;
        float intensity = (cosTheta - light.outerCutoff) / epsilon;

        diffuse *= intensity;
        specular *= intensity;
    }

    return ambient + diffuse + specular;
}
