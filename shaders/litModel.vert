#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 ourTexCoord;
out vec3 ourNormal;
out vec3 ourFragPos;
out vec3 ourViewPos;

uniform mat4 mvp;
uniform mat4 model;
uniform mat3 normal;
uniform vec3 viewPos;

void main()
{
   gl_Position = mvp * vec4(aPos, 1.0);
   ourTexCoord = vec2(aTexCoord.s, aTexCoord.t);
   ourFragPos = vec3(model * vec4(aPos, 1.0));
   ourNormal = normal * aNormal;
   ourViewPos = viewPos;
}