#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoords;

out vec2 textureCoords;
out vec3 normal;
out vec3 fragmentPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragmentPosition = vec3(model * vec4(aPosition, 1.0));
    textureCoords = aTextureCoords;
    normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}  
