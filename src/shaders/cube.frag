#version 330 core

out vec4 fragColor;  

// in vec2 textureCoords;
in vec3 normal;
in vec3 fragmentPosition;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

void main()
{
    // fragColor = mix(texture(texture1, textureCoords), texture(texture2, textureCoords), 0.2);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    float diffuseStrength = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;

    float specularStrength = 0.5;
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(result, 1.0);
}
