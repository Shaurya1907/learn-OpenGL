#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

const int MAX_POINT_LIGHTS = 3;

struct Light{
    vec3 colour;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight {
    Light base;
    vec3 direction;
};

struct PointLight{
    Light base;
    vec3 position;
    float constant;
    float linear;
    float exponent;
};

struct Material {
    float specularIntensity;
    float shininess;
};

uniform sampler2D theTexture;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform Material material;
uniform vec3 eyePosition;

uniform int pointLightCount;

vec3 CalcLightByDirection(Light light, vec3 direction)
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(direction);

    float diff = max(dot(N, L), 0.0);

    vec3 specularColour = vec3(0.0);
    if (diff > 0.0) {
        vec3 fragToEye = normalize(eyePosition - FragPos);
        vec3 reflectedVertex = normalize(reflect(-L, N));
        float specularFactor = max(dot(fragToEye, reflectedVertex), 0.0);
        if (specularFactor > 0.0) {
            specularFactor = pow(specularFactor, material.shininess);
            specularColour = light.colour * material.specularIntensity * specularFactor;
        }
    }

    vec3 ambient = light.colour * light.ambientIntensity;
    vec3 diffuse = light.colour * light.diffuseIntensity * diff;

    return ambient + diffuse + specularColour;
}


vec3 CalcDirectionalLight() {
    return CalcLightByDirection(directionalLight.base, directionalLight.direction);
}

vec3 CalcPointLight()
{
    vec3 totalColour = vec3(0.0);
    for (int i = 0; i < pointLightCount && i < MAX_POINT_LIGHTS; i++) {
        
        vec3 direction = pointLights[i].position - FragPos;
        float distance = length(direction);
        direction = direction / distance;

        vec3 lightColour = CalcLightByDirection(pointLights[i].base, direction);
        float attenuation = pointLights[i].exponent * distance * distance +
                            pointLights[i].linear * distance +
                            pointLights[i].constant;

        totalColour += lightColour / attenuation;
    }
    return totalColour;
}

void main() {
    vec3 finalColour = CalcDirectionalLight();
    finalColour += CalcPointLight();

    vec4 tex = texture(theTexture, TexCoord);
    FragColor = vec4(tex.rgb * finalColour, tex.a);
}