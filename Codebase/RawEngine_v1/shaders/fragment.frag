#version 400 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec3 backgroundColor;
uniform vec3 particleColor;

struct Screen
{
    int width;
    int height;
};
uniform Screen screen;

struct Particle
{
    vec2 position;
};

uniform int particleAmount;
uniform Particle particles[100];
uniform float particleRadius;

void main()
{
    // this way the particle makes use of screen pixel coordinates instead of texcoords. makes sure particles aren't affected by screensize.
    vec2 screenCoords = vec2(TexCoords.x * screen.width, TexCoords.y * screen.height);

    for(int i = 0; i < particleAmount; i++) // dont go over every particle in particles[] since that would create imaginary particles at 0.0, 0.0
    {
        if (length(particles[i].position - screenCoords) < particleRadius){ // if frag is in radius of a particle
            FragColor = vec4(particleColor, 1);
            break;
        }

        FragColor = vec4(backgroundColor, 1);
    }
}