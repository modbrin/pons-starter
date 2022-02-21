#version 450 core
out vec4 FragColor;

in vec2 ourTexCoord;
in vec3 ourNormal;
in vec3 ourFragPos;

uniform vec3 lightSourceColor;

void main()
{
    FragColor = vec4(lightSourceColor, 1.0);
}

