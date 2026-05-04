#version 430 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec3 particleColor1;
uniform vec3 particleColor2;
uniform vec2 velocity;

void main()
{
    FragColor = vec4(1,1,1,0);
    vec2 middleCoords = vec2(TexCoords.x - 0.5f, TexCoords.y - 0.5f);
    if (length(middleCoords) < 0.45f){
        vec3 lerpedColor = mix(particleColor1, particleColor2, length(velocity) / 5.0f); // lerp color based on the particles velocity
        FragColor = vec4(lerpedColor, 1);
    }
    if (FragColor.a == 0) {discard;}
}