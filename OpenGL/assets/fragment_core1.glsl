#version 330 core

out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture1;

uniform float mixVal;

void main() {
    //FragColor = vec4(ourColor, 1.0);
    //FragColor = texture(ourTexture, TexCoord);
    //FragColor = texture(ourTexture1, TexCoord);
    FragColor = mix(texture(ourTexture, TexCoord), texture(ourTexture1, TexCoord), mixVal);
}
