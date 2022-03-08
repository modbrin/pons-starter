#version 450 core
out vec4 FragColor;

in vec2 ourTexCoord;
in vec3 ourNormal;
in vec3 ourFragPos;
in vec3 ourViewPos;

void main()
{
    FragColor = vec4(0.04, 0.28, 0.26, 1.0);
}



