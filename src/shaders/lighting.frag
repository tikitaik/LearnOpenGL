#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 pos;
    vec3 direction;
    float cutoff;
    float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform Material material;
uniform Light light;

out vec4 FragColor;  
  
void main()
{
    // attenuation
    // float dist = length(light.pos - FragPos);
    // float attenuation = 1.0f / (light.constant + light.linear * dist + 
    //       light.quadratic * dist * dist);

    // ambient light
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    // diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.pos - FragPos);

    float cosTheta = dot(lightDir, normalize(-light.direction));

    // check for if in spotlight
    if (cosTheta > light.outerCutoff) {

        float diff = max(dot(norm, lightDir), 0.0);
        diffuse = light.diffuse * diff * 
            vec3(texture(material.diffuse, TexCoords));

        // specular light
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
        specular = light.specular * spec *
            vec3(texture(material.specular, TexCoords));
    }
    if (cosTheta < light.cutoff) {
        
        float epsilon = light.cutoff - light.outerCutoff;
        float intensity = (cosTheta - light.outerCutoff) / epsilon;

        diffuse *= intensity;
        specular *= intensity;
    }

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
