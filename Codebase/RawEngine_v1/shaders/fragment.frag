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
uniform Particle particles[100];
uniform float particleRadius;

void main()
{
    // this way the particle makes use of screen pixel coordinates instead of texcoords. makes sure particles aren't affected by screensize.
    vec2 screenCoords = vec2(TexCoords.x * screen.width, TexCoords.y * screen.height);

    for(int i = 0; i < particles.length(); i++)
    {
        if (length(particles[i].position - screenCoords) < particleRadius){ // if frag is in radius of a particle
            FragColor = vec4(particleColor, 1);
            break;
        }

        FragColor = vec4(backgroundColor, 1);
    }



    //if (length(particles[0].position - screenCoords) < particleRadius){
    //    FragColor = vec4(particleColor, 1);
    //}
    //else FragColor = vec4(backgroundColor, 1);

    //float dist = length(vec2(0.5, 0.5) - TexCoords);
    //FragColor = vec4(vec3(dist), 1.0);
}