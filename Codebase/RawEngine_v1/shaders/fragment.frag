#version 430 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec3 backgroundColor;
uniform vec3 particleColor1;
uniform vec3 particleColor2;

struct Screen
{
    int width;
    int height;
};
uniform Screen screen;

struct Particle
{
    vec2 position;
    vec2 velocity;
};

uniform int particleAmount;
uniform Particle particles[1000];
uniform float particleRadius;

void main()
{
    // this way the particle makes use of screen pixel coordinates instead of texcoords. makes sure particles aren't affected by screensize.
    vec2 screenCoords = vec2(TexCoords.x * screen.width, TexCoords.y * screen.height);
    FragColor = vec4(backgroundColor, 1);
    for(int i = 0; i < particleAmount; i++) // dont go over every particle in particles[] since that would create imaginary particles at 0.0, 0.0
    {
        if (length(particles[i].position - screenCoords) < particleRadius){ // if frag is in radius of a particle
            vec3 lerpedColor = mix(particleColor1, particleColor2, length(particles[i].velocity) / 5.0f); // lerp color based on the particles velocity
            FragColor = vec4(lerpedColor, 1);
            break;
        }
    }
}