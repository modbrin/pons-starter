#version 450 core
out vec4 FragColor;

in vec2 ourTexCoord;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform float mixRatio;

void main()
{
   FragColor = mix(texture(ourTexture, ourTexCoord), texture(ourTexture2, ourTexCoord), mixRatio);
}