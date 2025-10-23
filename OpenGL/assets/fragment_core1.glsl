#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D theTexture;

void main() {
    FragColor = texture(theTexture, TexCoord);
}
