#version 330 core
out vec4 FragColor;
in vec3 fPos;

void main()
{
   FragColor = vec4(fPos.x, fPos.y, fPos.z, 1);
}