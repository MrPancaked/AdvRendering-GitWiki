#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec2 Velocity;

uniform vec2 particlePos;
uniform float particleRad;
uniform vec2 screenSize;
uniform vec2 velocity;

void main()
{
    float particlePosX = 2.0f * aPos.x * particleRad / screenSize.x + particlePos.x;
    float particlePosY = 2.0f * aPos.y * particleRad / screenSize.y + particlePos.y;
    gl_Position = vec4(particlePosX, particlePosY, 0.0, 1.0);
    TexCoords = aTexCoords;
    Velocity = velocity;
}