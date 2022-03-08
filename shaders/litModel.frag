#version 450 core
out vec4 FragColor;

in vec2 ourTexCoord;
in vec3 ourNormal;
in vec3 ourFragPos;
in vec3 ourViewPos;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_normal1;
    sampler2D texture_normal2;
    sampler2D texture_height1;
    sampler2D texture_height2;
    sampler2D texture_emissive1;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
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

uniform Material material;
uniform DirectionalLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, ourTexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, ourTexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, ourTexCoord));
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));

    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, ourTexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, ourTexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, ourTexCoord));
    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));

    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, ourTexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, ourTexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, ourTexCoord));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    return (ambient + diffuse * intensity + specular * intensity) * attenuation;
}

void main()
{
    vec3 norm = normalize(ourNormal);
    vec3 viewDir = normalize(ourViewPos - ourFragPos);

    // phase 1: Directional lighting
    vec3 result = 0.1 * CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += 0.7 * CalcPointLight(pointLights[i], norm, ourFragPos, viewDir);
    }
    // phase 3: Spot light
//    result += CalcSpotLight(spotLight, norm, ourFragPos, viewDir);

    FragColor = vec4(result, 1.0); //+ texture(material.emissive, ourTexCoord).rgb;
}



