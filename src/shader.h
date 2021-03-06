#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <glm.hpp>
#include "utils/utils.h"

struct Shader
{
    u32 id;
};

internal char *GetShaderCode( char *path )
{
    FILE *shaderFile;
    fopen_s( &shaderFile, path, "rt" );

    if ( !shaderFile )
    {
        printf( "Failed to open file: %s.\n", path );
        return {};
    }

    fseek( shaderFile, 0, SEEK_END );
    int shaderFileSize = ( int ) ftell( shaderFile );
    fseek( shaderFile, 0, SEEK_SET );

    if ( shaderFileSize == 0 )
    {
        printf( "Vertex shader file size is 0.\n" );
        fclose( shaderFile );
        return {};
    }

    char *shaderCode = ( char * ) malloc( ( shaderFileSize + 1 ) * sizeof( char ) );
    int countRead = ( int ) fread( shaderCode, sizeof( char ), shaderFileSize, shaderFile );

    shaderCode[ countRead ] = '\0';
    fclose( shaderFile );
    return shaderCode;
}

internal Shader CreateShader( char *vertexPath, char *fragmentPath, char *geometryPath )
{
    int success;
    char infoLog[ 512 ];

    char *vertexCode = GetShaderCode( vertexPath );
    defer { free( vertexCode ); };

    char *fragmentCode = GetShaderCode( fragmentPath );
    defer { free( fragmentCode ); };

    u32 vertexShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertexShader, 1, &vertexCode, 0 );
    glCompileShader( vertexShader );
    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &success );

    if ( !success )
    {
        glGetShaderInfoLog( vertexShader, 512, 0, infoLog );
        printf( "Error during vertex shader compilation:\n%s", infoLog );
    }

    u32 fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragmentShader, 1, &fragmentCode, 0 );
    glCompileShader( fragmentShader );
    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &success );

    if ( !success )
    {
        glGetShaderInfoLog( fragmentShader, 512, 0, infoLog );
        printf( "Error during fragment shader compilation:\n%s", infoLog );
    }

    u32 id = glCreateProgram();
    glAttachShader( id, vertexShader );
    glAttachShader( id, fragmentShader );

    if ( geometryPath )
    {
        char *geometryCode = GetShaderCode( geometryPath );
        defer { free( geometryCode ); };

        u32 geometryShader = glCreateShader( GL_GEOMETRY_SHADER );
        glShaderSource( geometryShader, 1, &geometryCode, 0 );
        glCompileShader( geometryShader );
        glGetShaderiv( geometryShader, GL_COMPILE_STATUS, &success );

        if ( !success )
        {
            glGetShaderInfoLog( geometryShader, 512, 0, infoLog );
            printf( "Error during geometry shader compilation:\n%s", infoLog );
        }

        glAttachShader( id, geometryShader );
    }

    glLinkProgram( id );
    glGetProgramiv( id, GL_LINK_STATUS, &success );

    if ( !success )
    {
        glGetProgramInfoLog( id, 512, 0, infoLog );
        printf( "Error during shader linking:\n%s", infoLog );
    }

    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    return Shader{ id };
}

internal inline Shader CreateShader( char *vertexPath, char *fragmentPath )
{
    return CreateShader( vertexPath, fragmentPath, 0 );
}

inline void UseShader( Shader shader )
{
    glUseProgram( shader.id );
}

inline void BindUniformBlock( Shader shader, char *blockName, int bindingPoint )
{
    u32 blockIndex = glGetUniformBlockIndex( shader.id, blockName );
    glUniformBlockBinding( shader.id, blockIndex, bindingPoint );
}

internal int GetUniformLocation( int shader, char *name )
{
    int location = glGetUniformLocation( shader, name );
    if ( location < 0 )
    {
        printf( "Could not find shader location: %s\n", name );
    }
    return location;
}

inline void ShaderSetBool( Shader shader, char *name, bool value )
{
    glUniform1i( GetUniformLocation( shader.id, name ), ( int ) value );
}

inline void ShaderSetInt( Shader shader, char *name, int value )
{
    glUniform1i( GetUniformLocation( shader.id, name ), value );
}

inline void ShaderSetFloat32( Shader shader, char *name, float32 value )
{
    glUniform1f( GetUniformLocation( shader.id, name ), value );
}

inline void ShaderSetVec4( Shader shader, char *name, float32 x, float32 y, float32 z, float32 w )
{
    glUniform4f( GetUniformLocation( shader.id, name ), x, y, z, w );
}

inline void ShaderSetVec3( Shader shader, char *name, float32 x, float32 y, float32 z )
{
    glUniform3f( GetUniformLocation( shader.id, name ), x, y, z );
}

inline void ShaderSetVec3( Shader shader, char *name, glm::vec3 vec )
{
    glUniform3f( GetUniformLocation( shader.id, name ), vec.x, vec.y, vec.z );
}

inline void ShaderSetMat4( Shader shader, char *name, float32 *mat4 )
{
    glUniformMatrix4fv( GetUniformLocation( shader.id, name ), 1, GL_FALSE, mat4 );
}
