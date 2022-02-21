#version 450 core
out vec4 FragColor;

in vec2 ourTexCoord;
in vec3 ourNormal;
in vec3 ourFragPos;
in vec3 ourViewPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emissive;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = 0.3 * vec3(texture(material.diffuse, ourTexCoord)) * light.ambient;

    vec3 norm = normalize(ourNormal);
    vec3 lightDir = normalize(light.position - ourFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.8 * (vec3(texture(material.diffuse, ourTexCoord)) * diff) * light.diffuse;

    vec3 viewDir = normalize(ourViewPos - ourFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (vec3(texture(material.specular, ourTexCoord)) * spec) * light.specular;

    float distance = length(light.position - ourFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                        light.quadratic * (distance * distance));

    diffuse *= attenuation;
    specular *= attenuation;
    ambient *= attenuation;

    vec3 result = ambient + diffuse + specular; //+ texture(material.emissive, ourTexCoord).rgb;
    FragColor = vec4(result, 1.0);
}

