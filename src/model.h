#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <stack> //@TODO: Get rid of this
#include "mesh.h"
#include "stb_image.h"

struct Model
{
    Mesh *meshes;
    int meshesCount;

    char directory[ 128 ];
    std::vector< Texture > loadedTextures = {};
};

void DrawModel( Model model, Shader shader )
{
    for ( int i = 0; i < model.meshesCount; ++i )
    {
        DrawMesh( model.meshes[ i ], shader );
    }
}

u32 TextureFromFile( char *path, char *directory, bool gamma = false )
{
    const int bufferSize = 256;
    char filename[ bufferSize ] = "";
    char *separator = "/";
    strncat_s( filename, bufferSize, directory, strlen( directory ) );
    strncat_s( filename, bufferSize, separator, strlen( separator ) );
    strncat_s( filename, bufferSize, path, strlen( path ) );

    u32 id;
    glGenTextures( 1, &id );

    int width, height, componentsCount;
    u8 *data = stbi_load( filename, &width, &height, &componentsCount, 0 );

    if ( data )
    {
        GLenum format = 0;
        switch ( componentsCount )
        {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
        }

        glBindTexture( GL_TEXTURE_2D, id );
        glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data );
        glGenerateMipmap( GL_TEXTURE_2D );

        //@NOTE: GL_CLAMP_TO_EDGE for transparent not repeating textures, GL_REPEAT otherwise
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    else
    {
        printf( "Failed to load texture: %s\n", filename );
    }
    stbi_image_free( data );

    return id;
}

int LoadMaterialTextures( Model *model, aiMaterial *material, aiTextureType aiType, Texture_Type type, Texture *textures, int index )
{
    int result = 0;
    for ( u32 i = 0; i < material->GetTextureCount( aiType ); ++i )
    {
        aiString path;
        material->GetTexture( aiType, i, &path );

        bool skip = false;
        for ( u32 j = 0; j < model->loadedTextures.size(); ++j )
        {
            if ( strcmp( model->loadedTextures[ j ].path, path.C_Str() ) == 0 )
            {
                textures[ index + i ] = model->loadedTextures[ j ];
                skip = true;
                ++result;
                break;
            }
        }

        if ( skip ) { continue; }

        Texture texture;
        texture.id = TextureFromFile( ( char * ) path.C_Str(), model->directory );
        texture.type = type;
        strncpy_s( texture.path, path.C_Str(), path.length );
        texture.path[ path.length ] = '\0';
        textures[ index + i ] = texture;
        model->loadedTextures.push_back( texture );
        ++result;
    }
    return result;
}

Mesh ProcessMesh( Model *model, aiMesh *mesh, const aiScene *scene )
{
    Vertex *vertices = ( Vertex * ) malloc( mesh->mNumVertices * sizeof( Vertex ) );

    for ( u32 i = 0; i < mesh->mNumVertices; i++ )
    {
        Vertex vertex = {};
        vertex.position = glm::vec3( mesh->mVertices[ i ].x, mesh->mVertices[ i ].y, mesh->mVertices[ i ].z );
        vertex.normal = glm::vec3( mesh->mNormals[ i ].x, mesh->mNormals[ i ].y, mesh->mNormals[ i ].z );

        if ( mesh->mTextureCoords[ 0 ] )
        {
            vertex.textureCoordinates = glm::vec2( mesh->mTextureCoords[ 0 ][ i ].x, mesh->mTextureCoords[ 0 ][ i ].y );
        }
        else
        {
            vertex.textureCoordinates = glm::vec2( 0.0f, 0.0f );
        }

        vertices[ i ] = vertex;
    }

    u32 *indices = ( u32 * ) malloc( mesh->mNumFaces * 3 * sizeof( u32 ) );
    for ( u32 i = 0; i < mesh->mNumFaces; ++i )
    {
        aiFace face = mesh->mFaces[ i ];
        Assert( face.mNumIndices == 3 );
        for ( u32 j = 0; j < face.mNumIndices; ++j )
        {
            indices[ i * 3 + j ] = face.mIndices[ j ];
        }
    }

    Texture *textures = 0;
    int texturesCount = 0;
    if ( mesh->mMaterialIndex >= 0 )
    {
        aiMaterial *material = scene->mMaterials[ mesh->mMaterialIndex ];
        texturesCount = material->GetTextureCount( aiTextureType_DIFFUSE ) + material->GetTextureCount( aiTextureType_SPECULAR );
        textures = ( Texture * ) malloc( texturesCount * sizeof( Texture ) );
        int texturesLoaded = LoadMaterialTextures( model, material, aiTextureType_DIFFUSE, Texture_Type::DIFFUSE, textures, 0 );
        LoadMaterialTextures( model, material, aiTextureType_SPECULAR, Texture_Type::SPECULAR, textures, texturesLoaded );
    }

    return CreateMesh( vertices, mesh->mNumVertices, indices, mesh->mNumFaces * 3, textures, texturesCount );
}

void ProcessNodes( Model *model, const aiScene *scene )
{
    std::stack< aiNode * > stack;
    std::vector< aiNode * > visited;

    stack.push( scene->mRootNode );
    while ( !stack.empty() )
    {
        aiNode *temp = stack.top();
        stack.pop();
        visited.push_back( temp );
        for ( u32 i = 0; i < temp->mNumChildren; ++i )
        {
            stack.push( temp->mChildren[ i ] );
        }
    }

    int meshIndex = 0;
    for ( aiNode *node : visited )
    {
        for ( u32 i = 0; i < node->mNumMeshes; ++i )
        {
            aiMesh *mesh = scene->mMeshes[ node->mMeshes[ i ] ];
            model->meshes[ meshIndex++ ] = ProcessMesh( model, mesh, scene );
        }
    }
}

//@TODO: DeleteModel()
Model CreateModel( char *path )
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile( path, aiProcess_Triangulate | aiProcess_FlipUVs );

    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
    {
        printf( "Error during model loading: %s\n", importer.GetErrorString() );
        return {};
    }

    Model result = {};
    char *lastSlash = strrchr( path, '/' );
    if ( lastSlash )
    {
        int count = ( int ) ( lastSlash - path );
        strncpy_s( result.directory, path, count );
        result.directory[ count ] = '\0';
    }
    else
    {
        result.directory[ 0 ] = '.';
        result.directory[ 1 ] = '/';
        result.directory[ 2 ] = '\0';
    }

    result.meshes = ( Mesh * ) malloc( scene->mNumMeshes * sizeof( Mesh ) );
    result.meshesCount = scene->mNumMeshes;

    ProcessNodes( &result, scene );

    return result;
}
