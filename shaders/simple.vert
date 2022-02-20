#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 ourTexCoord;

uniform mat4 mvp;

void main()
{
   gl_Position = mvp * vec4(aPos, 1.0);
   ourTexCoord = vec2(aTexCoord.s, 1.0 - aTexCoord.t);
}