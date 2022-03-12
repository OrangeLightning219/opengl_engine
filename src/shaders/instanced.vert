#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoords;
layout (location = 3) in mat4 instanceMatrix;

out vec2 textureCoords;
out vec3 normal;
out vec3 fragmentPosition;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    fragmentPosition = vec3(instanceMatrix * vec4(aPosition, 1.0));
    textureCoords = aTextureCoords;
    normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;
    gl_Position = projection * view * instanceMatrix * vec4(aPosition, 1.0);
}  
