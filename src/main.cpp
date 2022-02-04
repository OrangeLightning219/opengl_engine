#include <stdio.h>
#include <glad/glad.h>
#include <glfw3.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "stb_image.h"
#include "utils/utils.h"
#include "shader.h"

void FramebufferSizeCallback( GLFWwindow *window, int width, int height )
{
    glViewport( 0, 0, width, height );
}

void ProcessInput( GLFWwindow *window )
{
    if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
    {
        glfwSetWindowShouldClose( window, true );
    }
}

int main()
{
    glfwInit();

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    int windowWidth = 1280;
    int windowHeight = 1280;
    GLFWwindow *window = glfwCreateWindow( windowWidth, windowHeight, "OpenGL Engine", 0, 0 );
    if ( !window )
    {
        printf( "Failed to create GLFW window!\n" );
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent( window );

    if ( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) )
    {
        printf( "Failed to initialize GLAD!\n" );
        return -1;
    }

    glViewport( 0, 0, windowWidth, windowHeight );
    glfwSetFramebufferSizeCallback( window, FramebufferSizeCallback );

    float vertices[] = {
        // positions      // colors         // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

    u32 indices[] = { 0, 1, 3,
                      1, 2, 3 };

    float32 textureCoords[] = { 0.0f, 0.0f,
                                1.0f, 0.0f,
                                0.5f, 1.0f };

    u32 texture1;
    glGenTextures( 1, &texture1 );
    glBindTexture( GL_TEXTURE_2D, texture1 );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    stbi_set_flip_vertically_on_load( true );

    int width, height, channelsCount;
    u8 *data = stbi_load( "container.jpg", &width, &height, &channelsCount, 0 );

    if ( data )
    {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
        glGenerateMipmap( GL_TEXTURE_2D );
        stbi_image_free( data );
    }
    else
    {
        printf( "Failed to load texture: container.jpg" );
    }

    u32 texture2;
    glGenTextures( 1, &texture2 );
    glBindTexture( GL_TEXTURE_2D, texture2 );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    data = stbi_load( "awesomeface.png", &width, &height, &channelsCount, 0 );

    if ( data )
    {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
        glGenerateMipmap( GL_TEXTURE_2D );
        stbi_image_free( data );
    }
    else
    {
        printf( "Failed to load texture: awesomeface.png" );
    }

    u32 vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    u32 vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

    u32 ebo;
    glGenBuffers( 1, &ebo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

    Shader shader = CreateShader( "../src/shaders/simple.vert", "../src/shaders/simple.frag" );

    UseShader( shader );
    ShaderSetInt( shader, "texture1", 0 );
    ShaderSetInt( shader, "texture2", 1 );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float32 ), 0 );
    glEnableVertexAttribArray( 0 );

    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float32 ), ( void * ) ( 3 * sizeof( float32 ) ) );
    glEnableVertexAttribArray( 1 );

    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( float32 ), ( void * ) ( 6 * sizeof( float32 ) ) );
    glEnableVertexAttribArray( 2 );

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    while ( !glfwWindowShouldClose( window ) )
    {
        ProcessInput( window );

        glClearColor( 0.4f, 0.4f, 0.4f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );

        glfwPollEvents();

        // float32 currentTime = ( float32 ) glfwGetTime();
        // float32 green = ( sinf( currentTime ) / 2.0f ) + 0.5f;
        // ShaderSetVec4( shader, "color", 0.0f, green, 0.0f, 1.0f );
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, texture1 );
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, texture2 );

        glBindVertexArray( vao );
        // glDrawArrays( GL_TRIANGLES, 0, 3 );
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
        glBindVertexArray( 0 );

        glfwSwapBuffers( window );
    }

    glfwTerminate();
    return 0;
}
