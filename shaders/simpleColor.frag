#version 450 core
out vec4 FragColor;

in vec2 ourTexCoord;
in vec3 ourNormal;
in vec3 ourFragPos;
in vec3 ourViewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = material.ambient * light.ambient;

    vec3 norm = normalize(ourNormal);
    vec3 lightDir = normalize(light.position - ourFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (material.diffuse * diff) * light.diffuse;

    vec3 viewDir = normalize(ourViewPos - ourFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (material.specular * spec) * light.specular;

    vec3 result = (ambient + diffuse + specular); // * objectColor
    FragColor = vec4(result, 1.0);
}

