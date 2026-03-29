#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec3 backgroundColor;
uniform vec3 particleColor;

uniform vec2 particlePosition;
uniform float particleRadius;

void main()
{
    //for(int i = 0; i < particles.length(); i++)
    //{
    //    if (abs(length(particles[i].position - fPos.xy)) < particleRadius){
    //        FragColor = vec4(0, 1, 1, 1);
    //        break;
    //    }
    //    FragColor = vec4(fPos.x, fPos.y, fPos.z, 1);
    //}

    if (abs(length(particlePosition - TexCoords)) < particleRadius){
        FragColor = vec4(particleColor, 1);
    }
    else FragColor = vec4(backgroundColor, 1);

    //float dist = length(vec2(0.5, 0.5) - TexCoords);
    //FragColor = vec4(vec3(dist), 1.0);
}