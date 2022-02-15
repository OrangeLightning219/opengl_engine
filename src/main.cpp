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

    stbi_set_flip_vertically_on_load( true );

    Shader modelShader = CreateShader( "../src/shaders/cube.vert", "../src/shaders/cube.frag" );
    Shader lightShader = CreateShader( "../src/shaders/cube.vert", "../src/shaders/light.frag" );

    UseShader( modelShader );
    ShaderSetFloat32( modelShader, "material.shininess", 32.0f );

    ShaderSetVec3( modelShader, "directionalLight.direction", -0.2f, -1.0f, -0.3f );
    ShaderSetVec3( modelShader, "directionalLight.ambient", 0.05f, 0.05f, 0.05f );
    ShaderSetVec3( modelShader, "directionalLight.diffuse", 0.4f, 0.4f, 0.4f );
    ShaderSetVec3( modelShader, "directionalLight.specular", 0.5f, 0.5f, 0.5f );

    // ShaderSetVec3( modelShader, "spotLight.position", camera.position );
    // ShaderSetVec3( modelShader, "spotLight.direction", camera.front );
    // ShaderSetVec3( modelShader, "spotLight.ambient", 0.0f, 0.0f, 0.0f );
    // ShaderSetVec3( modelShader, "spotLight.diffuse", 1.0f, 1.0f, 1.0f );
    // ShaderSetVec3( modelShader, "spotLight.specular", 1.0f, 1.0f, 1.0f );
    // ShaderSetFloat32( modelShader, "spotLight.cutOff", glm::cos( glm::radians( 12.5f ) ) );
    // ShaderSetFloat32( modelShader, "spotLight.outerCutOff", glm::cos( glm::radians( 17.5f ) ) );
    // ShaderSetFloat32( modelShader, "spotLight.constant", 1.0f );
    // ShaderSetFloat32( modelShader, "spotLight.linear", 0.09f );
    // ShaderSetFloat32( modelShader, "spotLight.quadratic", 0.032f );

    ShaderSetVec3( modelShader, "pointLights[0].position", 0.0f, 3.5f, 3.0f );
    ShaderSetVec3( modelShader, "pointLights[0].ambient", 0.05f, 0.05f, 0.05f );
    ShaderSetVec3( modelShader, "pointLights[0].diffuse", 0.8f, 0.8f, 0.8f );
    ShaderSetVec3( modelShader, "pointLights[0].specular", 1.0f, 1.0f, 1.0f );
    ShaderSetFloat32( modelShader, "pointLights[0].constant", 1.0f );
    ShaderSetFloat32( modelShader, "pointLights[0].linear", 0.02f );
    ShaderSetFloat32( modelShader, "pointLights[0].quadratic", 0.032f );

    Model backpack = CreateModel( "backpack/backpack.obj" );

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
        model = glm::mat4( 1.0f );
        ShaderSetMat4( modelShader, "model", glm::value_ptr( model ) );

        DrawModel( backpack, modelShader );

        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
