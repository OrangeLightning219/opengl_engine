#version 330 core

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

out vec4 fragmentColor;  

in vec2 textureCoords;
in vec3 normal;
in vec3 fragmentPosition;

uniform vec3 viewPosition;
uniform Material material;

#define POINT_LIGHTS_COUNT 1
uniform PointLight pointLights[POINT_LIGHTS_COUNT];
uniform DirectionalLight directionalLight;
uniform SpotLight spotLight;

uniform samplerCube skybox;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);

void main()
{
    vec4 textureColor = texture(material.texture_diffuse1, textureCoords);
    if(textureColor.a < 0.1)
    {
        discard;
    }
    vec3 norm = normalize(normal);
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);

    vec3 result = CalculateDirectionalLight(directionalLight, norm, viewDirection);

    for(int i = 0; i < POINT_LIGHTS_COUNT; ++i)
    {
        result += CalculatePointLight(pointLights[i], norm, fragmentPosition, viewDirection);
    }
    result += CalculateSpotLight(spotLight, norm, fragmentPosition, viewDirection);

    vec3 I = normalize(fragmentPosition - viewPosition);
    vec3 R = reflect(I, normalize(normal));
    fragmentColor = mix(vec4(result, textureColor.a), vec4(texture(skybox, R).rgb, 1.0), 0.01);
    /* fragmentColor = texture(material.texture_diffuse1, textureCoords); */
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(-light.direction);
    
    float diffuseStrength = max(dot(normal, lightDirection), 0.0);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specularStrength = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 diffuse = light.diffuse * diffuseStrength * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 specular = specularStrength * light.specular * vec3(texture(material.texture_specular1, textureCoords));

    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection)
{
    vec3 lightDirection = normalize(light.position - fragmentPosition);

    float diffuseStrength = max(dot(normal, lightDirection), 0.0);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specularStrength = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 diffuse = light.diffuse * diffuseStrength * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 specular = light.specular * specularStrength * vec3(texture(material.texture_specular1, textureCoords));

    float distance = length(light.position - fragmentPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection)
{
    vec3 lightDirection = normalize(light.position - fragmentPosition);
    float theta = dot(lightDirection, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float inensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float diffuseStrength = max(dot(normal, lightDirection), 0.0);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specularStrength = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 diffuse = light.diffuse * diffuseStrength * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 specular = light.specular * specularStrength * vec3(texture(material.texture_specular1, textureCoords));

    float distance = length(light.position - fragmentPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    return (ambient + diffuse * inensity + specular * inensity) * attenuation;
}
