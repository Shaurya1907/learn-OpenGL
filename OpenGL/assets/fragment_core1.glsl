#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

struct DirectionalLight {
    vec3 colour;
    float ambientIntensity;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;

void main() {
    vec3 ambientColour = directionalLight.colour * directionalLight.ambientIntensity;
    FragColor = vec4(ambientColour, 1.0f) * texture(theTexture, TexCoord);
}
