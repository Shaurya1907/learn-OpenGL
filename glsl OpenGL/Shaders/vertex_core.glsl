#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;
layout(location = 2) in vec3 norm;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 DirectionalLightSpacePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 directionalLightTransform;

void main() {
    TexCoord = aTex;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    DirectionalLightSpacePos = directionalLightTransform * model * vec4(aPos, 1.0);
    Normal = mat3(transpose(inverse(model))) * norm;
    FragPos = (model * vec4(aPos, 1.0)).xyz;
}