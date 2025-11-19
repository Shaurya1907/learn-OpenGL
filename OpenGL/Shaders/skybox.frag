#version 330 core

in vec3 TexCoords;
out vec4 Colour;

uniform samplerCube skybox;

void main()
{
    Colour = texture(skybox, TexCoords);
}