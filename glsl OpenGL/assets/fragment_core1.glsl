#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

struct DirectionalLight {
    vec3 colour;
    float ambientIntensity;

    vec3 direction;
    float diffuseIntensity;
};

struct Material {
    float specularIntensity;
    float shininess;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;
uniform Material material;
uniform vec3 eyePosition;

void main() {
    vec3 N = normalize(Normal);
    vec3 L = normalize(-directionalLight.direction); // opposite of light ray direction

    float diff = max(dot(N, L), 0.0);

    // specular as vec3
    vec3 specularColour = vec3(0.0);
    if (diff > 0.0) {
        vec3 fragToEye = normalize(eyePosition - FragPos);
        vec3 reflectedVertex = normalize(reflect(-L, N));
        float specularFactor = max(dot(fragToEye, reflectedVertex), 0.0);
        if (specularFactor > 0.0) {
            specularFactor = pow(specularFactor, material.shininess);
            specularColour = directionalLight.colour * material.specularIntensity * specularFactor;
        }
    }

    vec3 ambient = directionalLight.colour * directionalLight.ambientIntensity;
    vec3 diffuse = directionalLight.colour * directionalLight.diffuseIntensity * diff;

    vec3 lit = ambient + diffuse + specularColour;
    vec4 tex = texture(theTexture, TexCoord);
    FragColor = vec4(lit * tex.rgb, tex.a);
}