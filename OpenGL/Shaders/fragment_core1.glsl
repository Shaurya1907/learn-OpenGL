#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos;

out vec4 FragColor;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

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

struct SpotLight{
    PointLight base;
    vec3 direction;
    float edge;
};

struct Material {
    float specularIntensity;
    float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D theTexture;
uniform sampler2D directionalShadowMap;

uniform Material material;

uniform vec3 eyePosition;

float CalcDirectionalShadowFactor(DirectionalLight light)
{
    vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w;
    projCoords = (projCoords * 0.5) + 0.5;

    float closestDepth = texture(directionalShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.005;
    float shadowFactor = currentDepth > closestDepth + bias ? 1.0 : 0.0;   
    return shadowFactor;
}

vec3 CalcLightByDirection(Light light, vec3 direction, float shadowFactor)
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

    return ambient + (1.0 - shadowFactor) * (diffuse + specularColour);
}

vec3 CalcDirectionalLight() {
    float shadowFactor = CalcDirectionalShadowFactor(directionalLight);
    return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

vec3 CalcPointLight(PointLight pLight)
{
        vec3 direction = pLight.position - FragPos;
        float distance = length(direction);
        direction = direction / distance;

        vec3 lightColour = CalcLightByDirection(pLight.base, direction, 0.0f);
        float attenuation = pLight.exponent * distance * distance +
                            pLight.linear * distance +
                            pLight.constant;

        return lightColour / attenuation;
}

vec3 CalcSpotLight(SpotLight sLight)
{
    vec3 rayDirection = normalize(FragPos - sLight.base.position);
    float slFactor = dot(rayDirection, sLight.direction);

    if(slFactor > sLight.edge){
        vec3 colour = CalcPointLight(sLight.base);

        return colour * (1.0f - (1.0f - slFactor) * (1.0f / (1.0f - sLight.edge)));
    } else {
        return vec3(0.0);
    }
}

vec3 CalcPointLights()
{
    vec3 totalColour = vec3(0.0);
    for (int i = 0; i < pointLightCount && i < MAX_POINT_LIGHTS; i++) {
        
        totalColour += CalcPointLight(pointLights[i]);
    }
    return totalColour;
}

vec3 CalcSpotLights()
{
    vec3 totalColour = vec3(0.0);
    for (int i = 0; i < spotLightCount && i < MAX_SPOT_LIGHTS; i++) {
        totalColour += CalcSpotLight(spotLights[i]);
    }
    return totalColour;
}

void main() {
    vec3 finalColour = CalcDirectionalLight();
    finalColour += CalcPointLights();
    finalColour += CalcSpotLights();

    vec4 tex = texture(theTexture, TexCoord);
    FragColor = vec4(tex.rgb * finalColour, tex.a);
}