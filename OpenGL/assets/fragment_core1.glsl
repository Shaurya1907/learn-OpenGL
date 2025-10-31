#version 330 core

in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

struct DirectionalLight {
    vec3 colour;
    float ambientIntensity;

    vec3 direction;
    float diffuseIntensity;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;

void main() {
    vec3 N = normalize(Normal);
    vec3 L = normalize(-directionalLight.direction); // use -direction for light-to-surface

    float diff = max(dot(N, L), 0.0);

    vec3 ambient = directionalLight.colour * directionalLight.ambientIntensity;
    vec3 diffuse = directionalLight.colour * directionalLight.diffuseIntensity * diff;

    vec4 tex = texture(theTexture, TexCoord);
    FragColor = vec4((ambient + diffuse) * tex.rgb, tex.a);
}