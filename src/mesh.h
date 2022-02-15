#pragma once
#include <glm.hpp>
#include <glad/glad.h>
#include <stdio.h>
#include "utils/utils.h"
#include "shader.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoordinates;
};

enum Texture_Type
{
    DIFFUSE,
    SPECULAR
};

struct Texture
{
    u32 id;
    Texture_Type type;
    char *path;
};

struct Mesh
{
    Vertex *vertices;
    int verticesCount;

    u32 *indices;
    int indicesCount;

    Texture *textures;
    int texturesCount;

    u32 vao;
    u32 vbo;
    u32 ebo;
};

Mesh CreateMesh( Vertex *vertices, int verticesCount, u32 *indices, int indicesCount, Texture *textures, int texturesCount )
{
    Mesh result = {};

    result.vertices = vertices;
    result.verticesCount = verticesCount;
    result.indices = indices;
    result.indicesCount = indicesCount;
    result.textures = textures;
    result.texturesCount = texturesCount;

    glGenVertexArrays( 1, &result.vao );
    glGenBuffers( 1, &result.vbo );
    glGenBuffers( 1, &result.ebo );

    glBindVertexArray( result.vao );
    glBindBuffer( GL_ARRAY_BUFFER, result.vbo );
    glBufferData( GL_ARRAY_BUFFER, verticesCount * sizeof( Vertex ), vertices, GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, result.ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof( u32 ), indices, GL_STATIC_DRAW );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), 0 );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( void * ) offsetof( Vertex, normal ) );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( void * ) offsetof( Vertex, textureCoordinates ) );
    glEnableVertexAttribArray( 2 );

    glBindVertexArray( 0 );

    return result;
}

void DrawMesh( Mesh mesh, Shader shader )
{
    int diffuseNumber = 1;
    int specularNumber = 1;

    for ( int i = 0; i < mesh.texturesCount; ++i )
    {
        glActiveTexture( GL_TEXTURE0 + i );
        char fieldName[ 30 ];
        switch ( mesh.textures[ i ].type )
        {
            case Texture_Type::DIFFUSE:
            {
                snprintf( fieldName, sizeof( fieldName ), "material.texture_diffuse%d", diffuseNumber++ );
            }
            break;

            case Texture_Type::SPECULAR:
            {
                snprintf( fieldName, sizeof( fieldName ), "material.texture_specular%d", specularNumber++ );
            }
            break;
        }

        ShaderSetInt( shader, fieldName, i );
        glBindTexture( GL_TEXTURE_2D, mesh.textures[ i ].id );
    }

    glActiveTexture( GL_TEXTURE0 );

    glBindVertexArray( mesh.vao );
    glDrawElements( GL_TRIANGLES, mesh.indicesCount, GL_UNSIGNED_INT, 0 );
    glBindVertexArray( 0 );
}
