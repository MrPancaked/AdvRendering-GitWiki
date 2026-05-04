#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec2 Velocity;

uniform vec2 particlePosition;
uniform float particleRadius;
uniform vec2 screenSize;

void main()
{
    float particlePosX = 2.0f * aPos.x * particleRadius / screenSize.x + particlePosition.x;
    float particlePosY = 2.0f * aPos.y * particleRadius / screenSize.y + particlePosition.y;
    gl_Position = vec4(particlePosX, particlePosY, 0.0, 1.0);
    TexCoords = aTexCoords;
}