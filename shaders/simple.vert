#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 ourTexCoord;
out vec3 ourNormal;
out vec3 ourFragPos;

uniform mat4 mvp;
uniform mat4 model;
uniform mat3 normal;

void main()
{
   gl_Position = mvp * vec4(aPos, 1.0);
   ourTexCoord = vec2(aTexCoord.s, 1.0 - aTexCoord.t);
   ourFragPos = vec3(model * vec4(aPos, 1.0));
   ourNormal = normal * aNormal;
}