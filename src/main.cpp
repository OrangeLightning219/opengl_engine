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
#include "utils/utils.h"
#include "shader.h"
#include "camera.h"
#include "model.h"

global_variable float32 deltaTime = 0.0f;
global_variable float32 lastFrame = 0.0f;

global_variable bool firstMouse = true;
global_variable float32 lastMouseX = 0.0f;
global_variable float32 lastMouseY = 0.0f;

global_variable Camera camera;

global_variable int windowWidth = 1280;
global_variable int windowHeight = 1280;

void FramebufferSizeCallback( GLFWwindow *window, int width, int height )
{
    windowWidth = width;
    windowHeight = height;
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

    Shader modelShader = CreateShader( "../src/shaders/cube.vert", "../src/shaders/cube.frag" );
    Shader lightShader = CreateShader( "../src/shaders/cube.vert", "../src/shaders/light.frag" );
    Shader singleColorShader = CreateShader( "../src/shaders/cube.vert", "../src/shaders/single_color.frag" );

    UseShader( modelShader );
    ShaderSetFloat32( modelShader, "material.shininess", 32.0f );

    ShaderSetVec3( modelShader, "directionalLight.direction", -0.2f, -1.0f, -0.3f );
    ShaderSetVec3( modelShader, "directionalLight.ambient", 0.05f, 0.05f, 0.05f );
    ShaderSetVec3( modelShader, "directionalLight.diffuse", 0.4f, 0.4f, 0.4f );
    ShaderSetVec3( modelShader, "directionalLight.specular", 0.5f, 0.5f, 0.5f );

    ShaderSetVec3( modelShader, "spotLight.position", camera.position );
    ShaderSetVec3( modelShader, "spotLight.direction", camera.front );
    ShaderSetVec3( modelShader, "spotLight.ambient", 0.0f, 0.0f, 0.0f );
    ShaderSetVec3( modelShader, "spotLight.diffuse", 1.0f, 1.0f, 1.0f );
    ShaderSetVec3( modelShader, "spotLight.specular", 1.0f, 1.0f, 1.0f );
    ShaderSetFloat32( modelShader, "spotLight.cutOff", glm::cos( glm::radians( 12.5f ) ) );
    ShaderSetFloat32( modelShader, "spotLight.outerCutOff", glm::cos( glm::radians( 17.5f ) ) );
    ShaderSetFloat32( modelShader, "spotLight.constant", 1.0f );
    ShaderSetFloat32( modelShader, "spotLight.linear", 0.09f );
    ShaderSetFloat32( modelShader, "spotLight.quadratic", 0.032f );

    ShaderSetVec3( modelShader, "pointLights[0].position", 0.0f, 3.5f, 3.0f );
    ShaderSetVec3( modelShader, "pointLights[0].ambient", 0.05f, 0.05f, 0.05f );
    ShaderSetVec3( modelShader, "pointLights[0].diffuse", 0.8f, 0.8f, 0.8f );
    ShaderSetVec3( modelShader, "pointLights[0].specular", 1.0f, 1.0f, 1.0f );
    ShaderSetFloat32( modelShader, "pointLights[0].constant", 1.0f );
    ShaderSetFloat32( modelShader, "pointLights[0].linear", 0.02f );
    ShaderSetFloat32( modelShader, "pointLights[0].quadratic", 0.032f );

    stbi_set_flip_vertically_on_load( true );
    Model backpack = CreateModel( "backpack/backpack.obj" );
    stbi_set_flip_vertically_on_load( false );
    Model grass = CreateModel( "grass.obj" );
    Model redWindow = CreateModel( "red_window.obj" );

    u32 fbo;
    glGenFramebuffers( 1, &fbo );
    glBindFramebuffer( GL_FRAMEBUFFER, fbo );

    u32 framebufferTexture;
    glGenTextures( 1, &framebufferTexture );
    glBindTexture( GL_TEXTURE_2D, framebufferTexture );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0 );

    u32 rbo;
    glGenRenderbuffers( 1, &rbo );
    glBindRenderbuffer( GL_RENDERBUFFER, rbo );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight );
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );

    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo );

    if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
    {
        printf( "Creating frame buffer failed!\n" );
        return -1;
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                             // positions   // texCoords
                             -1.0f, 1.0f, 0.0f, 1.0f,
                             -1.0f, -1.0f, 0.0f, 0.0f,
                             1.0f, -1.0f, 1.0f, 0.0f,

                             -1.0f, 1.0f, 0.0f, 1.0f,
                             1.0f, -1.0f, 1.0f, 0.0f,
                             1.0f, 1.0f, 1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays( 1, &quadVAO );
    glGenBuffers( 1, &quadVBO );
    glBindVertexArray( quadVAO );
    glBindBuffer( GL_ARRAY_BUFFER, quadVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), &quadVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), ( void * ) 0 );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), ( void * ) ( 2 * sizeof( float ) ) );

    Shader quadShader = CreateShader( "../src/shaders/render_texture.vert", "../src/shaders/render_texture.frag" );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_STENCIL_TEST );
    glEnable( GL_BLEND );
    glEnable( GL_CULL_FACE );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    // glCullFace( GL_FRONT ); //GL_BACK is default
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); //Wireframe
    while ( !glfwWindowShouldClose( window ) )
    {
        float32 currentFrame = ( float32 ) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        ProcessInput( window );

        // glBindFramebuffer( GL_FRAMEBUFFER, fbo );
        glClearColor( 0.4f, 0.4f, 0.4f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

        glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );

        glm::mat4 view = GetViewMatrix( &camera );

        glm::mat4 projection = glm::mat4( 1.0f );
        projection = glm::perspective( glm::radians( camera.zoom ), ( float32 ) windowWidth / ( float32 ) windowHeight, 0.1f, 100.0f );

        UseShader( modelShader );
        ShaderSetMat4( modelShader, "projection", glm::value_ptr( projection ) );
        ShaderSetMat4( modelShader, "view", glm::value_ptr( view ) );
        ShaderSetVec3( modelShader, "viewPosition", camera.position );

        ShaderSetVec3( modelShader, "spotLight.position", camera.position );
        ShaderSetVec3( modelShader, "spotLight.direction", camera.front );

        glm::mat4 model = glm::mat4( 1.0f );
        ShaderSetMat4( modelShader, "model", glm::value_ptr( model ) );

        glStencilFunc( GL_ALWAYS, 1, 0xff );
        glStencilMask( 0xff );
        DrawModel( backpack, modelShader );

        glStencilFunc( GL_NOTEQUAL, 1, 0xff );
        glStencilMask( 0x00 );
        glDisable( GL_DEPTH_TEST );

        UseShader( singleColorShader );
        model = glm::scale( model, glm::vec3( 1.05f, 1.05f, 1.05f ) );
        ShaderSetMat4( singleColorShader, "model", glm::value_ptr( model ) );
        ShaderSetMat4( singleColorShader, "projection", glm::value_ptr( projection ) );
        ShaderSetMat4( singleColorShader, "view", glm::value_ptr( view ) );

        glEnable( GL_CULL_FACE );

        DrawModel( backpack, singleColorShader );

        glEnable( GL_DEPTH_TEST );
        glStencilFunc( GL_ALWAYS, 1, 0xff );
        glStencilMask( 0xff );

        glDisable( GL_CULL_FACE );

        UseShader( modelShader );
        glm::mat4 model1 = glm::mat4( 1.0f );
        model1 = glm::translate( model1, glm::vec3( 2.0f, 0.0f, 0.0f ) );
        ShaderSetMat4( modelShader, "model", glm::value_ptr( model1 ) );
        DrawModel( grass, modelShader );

        model1 = glm::translate( model1, glm::vec3( -2.0f, 0.0f, 2.0f ) );
        ShaderSetMat4( modelShader, "model", glm::value_ptr( model1 ) );
        DrawModel( redWindow, modelShader );

        model1 = glm::translate( model1, glm::vec3( -1.25f, 1.25f, 0.1f ) );
        ShaderSetMat4( modelShader, "model", glm::value_ptr( model1 ) );
        DrawModel( redWindow, modelShader );

        //         glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        //         glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
        //         glClear( GL_COLOR_BUFFER_BIT );

        //         UseShader( quadShader );
        //         glBindVertexArray( quadVAO );
        //         glDisable( GL_DEPTH_TEST );
        //         glBindTexture( GL_TEXTURE_2D, framebufferTexture );
        //         glDrawArrays( GL_TRIANGLES, 0, 6 );
        //         glEnable( GL_DEPTH_TEST );

        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glDeleteFramebuffers( 1, &fbo );

    glfwTerminate();
    return 0;
}
