#version 450 core
out vec4 FragColor;

in vec2 ourTexCoord;
in vec3 ourNormal;
in vec3 ourFragPos;
in vec3 ourLightPos;

uniform vec3 objectColor;
uniform vec3 lightColor;


void main()
{
    float ambientStrength = 0.1;
    float specularStrength = 0.5;
    vec3 ambient = ambientStrength * objectColor;
    vec3 norm = normalize(ourNormal);
    vec3 lightDir = normalize(ourLightPos - ourFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 viewDir = normalize(-ourFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}

