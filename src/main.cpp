#include <stdio.h>
#include <glad/glad.h>
#include <glfw3.h>
#include <math.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "stb_image.h"
#include "utils/utils.h"
#include "shader.h"
#include "camera.h"

global_variable float32 deltaTime = 0.0f;
global_variable float32 lastFrame = 0.0f;

global_variable bool firstMouse = true;
global_variable float32 lastMouseX = 0.0f;
global_variable float32 lastMouseY = 0.0f;

global_variable Camera camera;

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

    if ( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
    {
        CameraProcessKeyboard( &camera, FORWARD, deltaTime );
    }

    if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
    {
        CameraProcessKeyboard( &camera, BACKWARD, deltaTime );
    }

    if ( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )
    {
        CameraProcessKeyboard( &camera, LEFT, deltaTime );
    }

    if ( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )
    {
        CameraProcessKeyboard( &camera, RIGHT, deltaTime );
    }
}

void MouseCallback( GLFWwindow *window, float64 mouseX, float64 mouseY )
{
    if ( firstMouse )
    {
        lastMouseX = ( float32 ) mouseX;
        lastMouseY = ( float32 ) mouseY;
        firstMouse = false;
    }

    float32 xOffset = ( float32 ) mouseX - lastMouseX;
    float32 yOffset = lastMouseY - ( float32 ) mouseY;
    lastMouseX = ( float32 ) mouseX;
    lastMouseY = ( float32 ) mouseY;
    CameraProcessMouse( &camera, xOffset, yOffset );
}

void ScrollCallback( GLFWwindow *window, float64 xOffset, float64 yOffset )
{
    CameraProcessScroll( &camera, ( float32 ) yOffset );
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

    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    glfwSetCursorPosCallback( window, MouseCallback );
    glfwSetScrollCallback( window, ScrollCallback );
    glfwMakeContextCurrent( window );

    if ( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) )
    {
        printf( "Failed to initialize GLAD!\n" );
        return -1;
    }

    glViewport( 0, 0, windowWidth, windowHeight );
    glfwSetFramebufferSizeCallback( window, FramebufferSizeCallback );

    camera = CreateCamera( glm::vec3( 0.0f, 3.0f, 6.0f ), defaultYaw, -20.0f );

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f, 0.0f, 0.0f ),
        glm::vec3( 2.0f, 5.0f, -15.0f ),
        glm::vec3( -1.5f, -2.2f, -2.5f ),
        glm::vec3( -3.8f, -2.0f, -12.3f ),
        glm::vec3( 2.4f, -0.4f, -3.5f ),
        glm::vec3( -1.7f, 3.0f, -7.5f ),
        glm::vec3( 1.3f, -2.0f, -2.5f ),
        glm::vec3( 1.5f, 2.0f, -2.5f ),
        glm::vec3( 1.5f, 0.2f, -1.5f ),
        glm::vec3( -1.3f, 1.0f, -1.5f )
    };
    float vertices[] = {
        // positions         // normals         // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    u32 indices[] = { 0, 1, 3,
                      1, 2, 3 };

    float32 textureCoords[] = { 0.0f, 0.0f,
                                1.0f, 0.0f,
                                0.5f, 1.0f };

    u32 diffuseMap;
    glGenTextures( 1, &diffuseMap );
    glBindTexture( GL_TEXTURE_2D, diffuseMap );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    stbi_set_flip_vertically_on_load( true );

    int width, height, channelsCount;
    u8 *data = stbi_load( "container2.png", &width, &height, &channelsCount, 0 );

    if ( data )
    {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
        glGenerateMipmap( GL_TEXTURE_2D );
        stbi_image_free( data );
    }
    else
    {
        printf( "Failed to load texture: container2.png" );
    }

    u32 specularMap;
    glGenTextures( 1, &specularMap );
    glBindTexture( GL_TEXTURE_2D, specularMap );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    stbi_set_flip_vertically_on_load( true );

    width, height, channelsCount;
    data = stbi_load( "container2_specular.png", &width, &height, &channelsCount, 0 );

    if ( data )
    {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
        glGenerateMipmap( GL_TEXTURE_2D );
        stbi_image_free( data );
    }
    else
    {
        printf( "Failed to load texture: container2_specular.png" );
    }
    u32 vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    u32 vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float32 ), 0 );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float32 ), ( void * ) ( 3 * sizeof( float32 ) ) );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( float32 ), ( void * ) ( 6 * sizeof( float32 ) ) );
    glEnableVertexAttribArray( 2 );

    u32 lightVAO;
    glGenVertexArrays( 1, &lightVAO );
    glBindVertexArray( lightVAO );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float32 ), 0 );
    glEnableVertexAttribArray( 0 );
    // glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float32 ), ( void * ) ( 3 * sizeof( float32 ) ) );
    // glEnableVertexAttribArray( 1 );
    // u32 ebo;
    // glGenBuffers( 1, &ebo );
    // glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
    // glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

    Shader cubeShader = CreateShader( "../src/shaders/cube.vert", "../src/shaders/cube.frag" );
    Shader lightShader = CreateShader( "../src/shaders/cube.vert", "../src/shaders/light.frag" );

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f, 0.2f, 2.0f ),
        glm::vec3( 2.3f, -3.3f, -4.0f ),
        glm::vec3( -4.0f, 2.0f, -12.0f ),
        glm::vec3( 0.0f, 0.0f, -3.0f )
    };
    UseShader( cubeShader );
    ShaderSetInt( cubeShader, "material.diffuse", 0 );
    ShaderSetInt( cubeShader, "material.specular", 1 );
    ShaderSetFloat32( cubeShader, "material.shininess", 64.0f );

    ShaderSetVec3( cubeShader, "directionalLight.direction", -0.2f, -1.0f, -0.3f );
    ShaderSetVec3( cubeShader, "directionalLight.ambient", 0.05f, 0.05f, 0.05f );
    ShaderSetVec3( cubeShader, "directionalLight.diffuse", 0.4f, 0.4f, 0.4f );
    ShaderSetVec3( cubeShader, "directionalLight.specular", 0.5f, 0.5f, 0.5f );

    ShaderSetVec3( cubeShader, "spotLight.position", camera.position );
    ShaderSetVec3( cubeShader, "spotLight.direction", camera.front );
    ShaderSetVec3( cubeShader, "spotLight.ambient", 0.0f, 0.0f, 0.0f );
    ShaderSetVec3( cubeShader, "spotLight.diffuse", 1.0f, 1.0f, 1.0f );
    ShaderSetVec3( cubeShader, "spotLight.specular", 1.0f, 1.0f, 1.0f );
    ShaderSetFloat32( cubeShader, "spotLight.cutOff", glm::cos( glm::radians( 12.5f ) ) );
    ShaderSetFloat32( cubeShader, "spotLight.outerCutOff", glm::cos( glm::radians( 17.5f ) ) );
    ShaderSetFloat32( cubeShader, "spotLight.constant", 1.0f );
    ShaderSetFloat32( cubeShader, "spotLight.linear", 0.09f );
    ShaderSetFloat32( cubeShader, "spotLight.quadratic", 0.032f );

    ShaderSetVec3( cubeShader, "pointLights[0].position", pointLightPositions[ 0 ] );
    ShaderSetVec3( cubeShader, "pointLights[0].ambient", 0.05f, 0.05f, 0.05f );
    ShaderSetVec3( cubeShader, "pointLights[0].diffuse", 0.8f, 0.8f, 0.8f );
    ShaderSetVec3( cubeShader, "pointLights[0].specular", 1.0f, 1.0f, 1.0f );
    ShaderSetFloat32( cubeShader, "pointLights[0].constant", 1.0f );
    ShaderSetFloat32( cubeShader, "pointLights[0].linear", 0.02f );
    ShaderSetFloat32( cubeShader, "pointLights[0].quadratic", 0.032f );

    ShaderSetVec3( cubeShader, "pointLights[1].position", pointLightPositions[ 1 ] );
    ShaderSetVec3( cubeShader, "pointLights[1].ambient", 0.05f, 0.05f, 0.05f );
    ShaderSetVec3( cubeShader, "pointLights[1].diffuse", 0.8f, 0.8f, 0.8f );
    ShaderSetVec3( cubeShader, "pointLights[1].specular", 1.0f, 1.0f, 1.0f );
    ShaderSetFloat32( cubeShader, "pointLights[1].constant", 1.0f );
    ShaderSetFloat32( cubeShader, "pointLights[1].linear", 0.02f );
    ShaderSetFloat32( cubeShader, "pointLights[1].quadratic", 0.032f );

    ShaderSetVec3( cubeShader, "pointLights[2].position", pointLightPositions[ 2 ] );
    ShaderSetVec3( cubeShader, "pointLights[2].ambient", 0.05f, 0.05f, 0.05f );
    ShaderSetVec3( cubeShader, "pointLights[2].diffuse", 0.8f, 0.8f, 0.8f );
    ShaderSetVec3( cubeShader, "pointLights[2].specular", 1.0f, 1.0f, 1.0f );
    ShaderSetFloat32( cubeShader, "pointLights[2].constant", 1.0f );
    ShaderSetFloat32( cubeShader, "pointLights[2].linear", 0.02f );
    ShaderSetFloat32( cubeShader, "pointLights[2].quadratic", 0.032f );

    ShaderSetVec3( cubeShader, "pointLights[3].position", pointLightPositions[ 3 ] );
    ShaderSetVec3( cubeShader, "pointLights[3].ambient", 0.05f, 0.05f, 0.05f );
    ShaderSetVec3( cubeShader, "pointLights[3].diffuse", 0.8f, 0.8f, 0.8f );
    ShaderSetVec3( cubeShader, "pointLights[3].specular", 1.0f, 1.0f, 1.0f );
    ShaderSetFloat32( cubeShader, "pointLights[3].constant", 1.0f );
    ShaderSetFloat32( cubeShader, "pointLights[3].linear", 0.02f );
    ShaderSetFloat32( cubeShader, "pointLights[3].quadratic", 0.032f );
    glEnable( GL_DEPTH_TEST );

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    while ( !glfwWindowShouldClose( window ) )
    {
        float32 currentFrame = ( float32 ) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        ProcessInput( window );

        glClearColor( 0.4f, 0.4f, 0.4f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, diffuseMap );
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, specularMap );

        glm::mat4 view = GetViewMatrix( &camera );

        glm::mat4 projection = glm::mat4( 1.0f );
        projection = glm::perspective( glm::radians( camera.zoom ), ( float32 ) windowWidth / ( float32 ) windowHeight, 0.1f, 100.0f );

        UseShader( cubeShader );
        ShaderSetMat4( cubeShader, "projection", glm::value_ptr( projection ) );
        ShaderSetMat4( cubeShader, "view", glm::value_ptr( view ) );
        ShaderSetVec3( cubeShader, "viewPosition", camera.position );

        UseShader( lightShader );
        ShaderSetMat4( lightShader, "projection", glm::value_ptr( projection ) );
        ShaderSetMat4( lightShader, "view", glm::value_ptr( view ) );

        glBindVertexArray( lightVAO );
        glm::mat4 model;
        for ( int i = 0; i < 4; ++i )
        {
            model = glm::mat4( 1.0f );
            model = glm::translate( model, pointLightPositions[ i ] );
            model = glm::scale( model, glm::vec3( 0.2f ) );
            ShaderSetMat4( lightShader, "model", glm::value_ptr( model ) );
            glDrawArrays( GL_TRIANGLES, 0, 36 );
        }

        UseShader( cubeShader );
        ShaderSetVec3( cubeShader, "spotLight.position", camera.position );
        ShaderSetVec3( cubeShader, "spotLight.direction", camera.front );

        //         glm::vec3 lightColor;
        //         lightColor.x = sinf( ( float32 ) glfwGetTime() * 2.0f );
        //         lightColor.y = sinf( ( float32 ) glfwGetTime() * 0.7f );
        //         lightColor.z = sinf( ( float32 ) glfwGetTime() * 1.3f );
        //         glm::vec3 diffuseColor = lightColor * glm::vec3( 0.5f );
        //         glm::vec3 ambientColor = lightColor * glm::vec3( 0.2f );

        //         ShaderSetVec3( cubeShader, "light.ambient", ambientColor );
        //         ShaderSetVec3( cubeShader, "light.diffuse", diffuseColor );

        glBindVertexArray( vao );

        for ( int i = 0; i < 10; ++i )
        {
            model = glm::mat4( 1.0f );
            model = glm::translate( model, cubePositions[ i ] );
            float angle = 20.0f * i;
            model = glm::rotate( model, glm::radians( angle ), glm::vec3( 1.0f, 0.3f, 0.5f ) );
            ShaderSetMat4( cubeShader, "model", glm::value_ptr( model ) );

            glDrawArrays( GL_TRIANGLES, 0, 36 );
        }
        // glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
        glBindVertexArray( 0 );

        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
