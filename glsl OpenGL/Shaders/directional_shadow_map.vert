#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 directionalLightTransform; // exact name

void main()
{
    gl_Position = directionalLightTransform * model * vec4(aPos, 1.0);
}