#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 ourTexCoord;
out vec3 ourNormal;
out vec3 ourFragPos;
out vec3 ourLightPos;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 view;
uniform mat3 normal;
uniform vec3 lightPos;

void main()
{
   gl_Position = mvp * vec4(aPos, 1.0);
   ourTexCoord = vec2(aTexCoord.s, 1.0 - aTexCoord.t);
   ourFragPos = vec3(mv * vec4(aPos, 1.0));
   ourNormal = normal * aNormal;
   ourLightPos = vec3(view * vec4(lightPos, 1.0));
}