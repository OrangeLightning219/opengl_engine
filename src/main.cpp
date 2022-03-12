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

GLenum glCheckError( char *file, int line )
{
    GLenum errorCode;
    while ( ( errorCode = glGetError() ) != GL_NO_ERROR )
    {
        char *error = 0;
        switch ( errorCode )
        {
            case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW: error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW: error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        printf( "%s | %s (line: %d)", error, file, line );
    }
    return errorCode;
}
#define glCheckError() glCheckError_( __FILE__, __LINE__ )

void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
                             const char *message, const void *userParam )
{
    // ignore non-significant error/warning codes
    if ( id == 131169 || id == 131185 || id == 131218 || id == 131204 ) return;

    char *sourceString = 0;
    switch ( source )
    {
        case GL_DEBUG_SOURCE_API: sourceString = "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceString = "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceString = "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: sourceString = "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: sourceString = "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER: sourceString = "Source: Other"; break;
    }

    char *typeString = 0;
    switch ( type )
    {
        case GL_DEBUG_TYPE_ERROR: typeString = "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeString = "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeString = "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY: typeString = "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: typeString = "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER: typeString = "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: typeString = "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP: typeString = "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER: typeString = "Type: Other"; break;
    }

    char *severityString = 0;
    switch ( severity )
    {
        case GL_DEBUG_SEVERITY_HIGH: severityString = "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: severityString = "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW: severityString = "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severityString = "Severity: notification"; break;
    }
    printf( "Debug message (%d): %s\n%s | %s | %s\n\n", id, message, sourceString, typeString, severityString );
}

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

    if ( glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS )
    {
        camera.movementSpeed = 7.0f;
    }
    else
    {
        camera.movementSpeed = defaultSpeed;
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

u32 loadCubemap( char *faces[ 6 ] )
{
    u32 id;
    glGenTextures( 1, &id );
    glBindTexture( GL_TEXTURE_CUBE_MAP, id );

    int width, height, channelCount;

    for ( int i = 0; i < 6; ++i )
    {
        u8 *data = stbi_load( faces[ i ], &width, &height, &channelCount, 0 );
        if ( data )
        {
            glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
        }
        else
        {
            printf( "Failed to load cubemap texture: %s\n", faces[ i ] );
        }
        stbi_image_free( data );
    }

    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

    return id;
}

int main()
{
    glfwInit();

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, true );

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

    int flags;
    glGetIntegerv( GL_CONTEXT_FLAGS, &flags );
    if ( flags & GL_CONTEXT_FLAG_DEBUG_BIT )
    {
        glEnable( GL_DEBUG_OUTPUT );
        glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
        glDebugMessageCallback( glDebugOutput, 0 );
        glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE );
    }

    glViewport( 0, 0, windowWidth, windowHeight );
    glfwSetFramebufferSizeCallback( window, FramebufferSizeCallback );

    camera = CreateCamera( glm::vec3( 0.0f, 3.0f, 6.0f ), defaultYaw, -20.0f );

    Shader modelShader = CreateShader( "../src/shaders/cube.vert", "../src/shaders/cube.frag" );
    Shader lightShader = CreateShader( "../src/shaders/cube.vert", "../src/shaders/light.frag" );
    Shader singleColorShader = CreateShader( "../src/shaders/cube.vert", "../src/shaders/single_color.frag" );
    Shader quadShader = CreateShader( "../src/shaders/render_texture.vert", "../src/shaders/render_texture.frag" );
    Shader skyboxShader = CreateShader( "../src/shaders/skybox.vert", "../src/shaders/skybox.frag" );
    Shader instancedShader = CreateShader( "../src/shaders/instanced.vert", "../src/shaders/cube.frag" );

    BindUniformBlock( modelShader, "Matrices", 0 );
    BindUniformBlock( instancedShader, "Matrices", 0 );

    u32 uboMatrices;
    glGenBuffers( 1, &uboMatrices );
    glBindBuffer( GL_UNIFORM_BUFFER, uboMatrices );
    glBufferData( GL_UNIFORM_BUFFER, 2 * sizeof( glm::mat4 ), 0, GL_STATIC_DRAW );
    glBindBuffer( GL_UNIFORM_BUFFER, 0 );

    glBindBufferRange( GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof( glm::mat4 ) );

    glm::mat4 projection = glm::perspective( glm::radians( 45.0f ), ( float32 ) windowWidth / ( float32 ) windowHeight, 0.1f, 500.0f );
    glBindBuffer( GL_UNIFORM_BUFFER, uboMatrices );
    glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof( glm::mat4 ), glm::value_ptr( projection ) );
    glBindBuffer( GL_UNIFORM_BUFFER, 0 );

    UseShader( skyboxShader );
    ShaderSetMat4( skyboxShader, "projection", glm::value_ptr( projection ) );

    Shader shaders[] = { modelShader, instancedShader };

    for ( Shader shader : shaders )
    {
        UseShader( shader );
        ShaderSetFloat32( shader, "material.shininess", 32.0f );

        ShaderSetVec3( shader, "directionalLight.direction", -0.2f, -1.0f, -0.3f );
        ShaderSetVec3( shader, "directionalLight.ambient", 0.05f, 0.05f, 0.05f );
        ShaderSetVec3( shader, "directionalLight.diffuse", 0.4f, 0.4f, 0.4f );
        ShaderSetVec3( shader, "directionalLight.specular", 0.5f, 0.5f, 0.5f );

        ShaderSetVec3( shader, "spotLight.position", camera.position );
        ShaderSetVec3( shader, "spotLight.direction", camera.front );
        ShaderSetVec3( shader, "spotLight.ambient", 0.0f, 0.0f, 0.0f );
        ShaderSetVec3( shader, "spotLight.diffuse", 1.0f, 1.0f, 1.0f );
        ShaderSetVec3( shader, "spotLight.specular", 1.0f, 1.0f, 1.0f );
        ShaderSetFloat32( shader, "spotLight.cutOff", glm::cos( glm::radians( 12.5f ) ) );
        ShaderSetFloat32( shader, "spotLight.outerCutOff", glm::cos( glm::radians( 17.5f ) ) );
        ShaderSetFloat32( shader, "spotLight.constant", 1.0f );
        ShaderSetFloat32( shader, "spotLight.linear", 0.09f );
        ShaderSetFloat32( shader, "spotLight.quadratic", 0.032f );

        ShaderSetVec3( shader, "pointLights[0].position", 0.0f, 3.5f, 3.0f );
        ShaderSetVec3( shader, "pointLights[0].ambient", 0.05f, 0.05f, 0.05f );
        ShaderSetVec3( shader, "pointLights[0].diffuse", 0.8f, 0.8f, 0.8f );
        ShaderSetVec3( shader, "pointLights[0].specular", 1.0f, 1.0f, 1.0f );
        ShaderSetFloat32( shader, "pointLights[0].constant", 1.0f );
        ShaderSetFloat32( shader, "pointLights[0].linear", 0.02f );
        ShaderSetFloat32( shader, "pointLights[0].quadratic", 0.032f );
    }

    stbi_set_flip_vertically_on_load( true );
    Model backpack = CreateModel( "backpack/backpack.obj" );
    stbi_set_flip_vertically_on_load( false );
    Model grass = CreateModel( "grass.obj" );
    Model redWindow = CreateModel( "red_window.obj" );

    Model planet = CreateModel( "planet/planet.obj" );
    Model asteroid = CreateModel( "rock/rock.obj" );

    const int asteroidCount = 250000;
    glm::mat4 *asteroidMatrices = ( glm::mat4 * ) malloc( asteroidCount * sizeof( glm::mat4 ) );
    srand( ( u32 ) glfwGetTime() );
    float32 radius = 150.0f;
    float32 offset = 25.0f;

    for ( int i = 0; i < asteroidCount; ++i )
    {
        glm::mat4 asteroidModel = glm::mat4( 1.0f );
        float32 angle = ( ( float32 ) i / ( float32 ) asteroidCount ) * 360.0f;
        float32 displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
        float32 x = sin( angle ) * radius + displacement;
        displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
        float32 y = displacement * 0.4f;
        displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
        float32 z = cos( angle ) * radius + displacement;
        asteroidModel = glm::translate( asteroidModel, glm::vec3( x, y, z ) );

        float32 scale = ( rand() % 20 ) / 100.0f + 0.05f;
        asteroidModel = glm::scale( asteroidModel, glm::vec3( scale ) );

        float32 rotation = ( float32 ) ( rand() % 360 );
        asteroidModel = glm::rotate( asteroidModel, rotation, glm::vec3( 0.4f, 0.6f, 0.8f ) );

        asteroidMatrices[ i ] = asteroidModel;
    }

    u32 asteroidBuffer;
    glGenBuffers( 1, &asteroidBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, asteroidBuffer );
    glBufferData( GL_ARRAY_BUFFER, asteroidCount * sizeof( glm::mat4 ), asteroidMatrices, GL_STATIC_DRAW );

    for ( int i = 0; i < asteroid.meshesCount; ++i )
    {
        u32 vao = asteroid.meshes[ i ].vao;
        glBindVertexArray( vao );
        u32 vec4Size = sizeof( glm::vec4 );
        glEnableVertexAttribArray( 3 );
        glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, ( void * ) 0 );
        glEnableVertexAttribArray( 4 );
        glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, ( void * ) ( 1 * vec4Size ) );
        glEnableVertexAttribArray( 5 );
        glVertexAttribPointer( 5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, ( void * ) ( 2 * vec4Size ) );
        glEnableVertexAttribArray( 6 );
        glVertexAttribPointer( 6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, ( void * ) ( 3 * vec4Size ) );

        glVertexAttribDivisor( 3, 1 );
        glVertexAttribDivisor( 4, 1 );
        glVertexAttribDivisor( 5, 1 );
        glVertexAttribDivisor( 6, 1 );

        glBindVertexArray( 0 );
    }

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
    u32 quadVAO, quadVBO;
    glGenVertexArrays( 1, &quadVAO );
    glGenBuffers( 1, &quadVBO );
    glBindVertexArray( quadVAO );
    glBindBuffer( GL_ARRAY_BUFFER, quadVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), &quadVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), ( void * ) 0 );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), ( void * ) ( 2 * sizeof( float ) ) );

    float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    u32 skyboxVAO, skyboxVBO;
    glGenVertexArrays( 1, &skyboxVAO );
    glGenBuffers( 1, &skyboxVBO );
    glBindVertexArray( skyboxVAO );
    glBindBuffer( GL_ARRAY_BUFFER, skyboxVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( skyboxVertices ), &skyboxVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), ( void * ) 0 );

    char *cubemapFaces[ 6 ] = { "skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg",
                                "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg" };
    u32 cubemapTexture = loadCubemap( cubemapFaces );

    glEnable( GL_DEPTH_TEST );
    // glEnable( GL_STENCIL_TEST );
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

        glEnable( GL_CULL_FACE );
        // glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );

        glm::mat4 view = GetViewMatrix( &camera );
        glBindBuffer( GL_UNIFORM_BUFFER, uboMatrices );
        glBufferSubData( GL_UNIFORM_BUFFER, sizeof( glm::mat4 ), sizeof( glm::mat4 ), glm::value_ptr( view ) );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );

        UseShader( modelShader );
        ShaderSetVec3( modelShader, "viewPosition", camera.position );

        ShaderSetVec3( modelShader, "spotLight.position", camera.position );
        ShaderSetVec3( modelShader, "spotLight.direction", camera.front );

        glm::mat4 model = glm::mat4( 1.0f );
        model = glm::scale( model, glm::vec3( 4.0f, 4.0f, 4.0f ) );
        ShaderSetMat4( modelShader, "model", glm::value_ptr( model ) );

        // glStencilFunc( GL_ALWAYS, 1, 0xff );
        // glStencilMask( 0xff );
        glActiveTexture( GL_TEXTURE5 );
        ShaderSetInt( modelShader, "skybox", 5 );
        glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture );
        // DrawModel( backpack, modelShader );

        DrawModel( planet, modelShader );

        UseShader( instancedShader );
        ShaderSetInt( instancedShader, "skybox", 5 );
        ShaderSetVec3( instancedShader, "viewPosition", camera.position );
        ShaderSetVec3( instancedShader, "spotLight.position", camera.position );
        ShaderSetVec3( instancedShader, "spotLight.direction", camera.front );

        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, asteroid.loadedTextures[ 0 ].id );
        ShaderSetInt( instancedShader, "material.texture_diffuse1", 0 );
        for ( int i = 0; i < asteroid.meshesCount; ++i )
        {
            glBindVertexArray( asteroid.meshes[ i ].vao );
            glDrawElementsInstanced( GL_TRIANGLES, asteroid.meshes[ i ].indicesCount, GL_UNSIGNED_INT, 0, asteroidCount );
            glBindVertexArray( 0 );
        }

        // glStencilFunc( GL_NOTEQUAL, 1, 0xff );
        // glStencilMask( 0x00 );
        // glDisable( GL_DEPTH_TEST );

        // UseShader( singleColorShader );
        // model = glm::scale( model, glm::vec3( 1.05f, 1.05f, 1.05f ) );
        // ShaderSetMat4( singleColorShader, "model", glm::value_ptr( model ) );
        // ShaderSetMat4( singleColorShader, "projection", glm::value_ptr( projection ) );
        // ShaderSetMat4( singleColorShader, "view", glm::value_ptr( view ) );

        //         DrawModel( backpack, singleColorShader );

        //         glEnable( GL_DEPTH_TEST );
        //         glStencilFunc( GL_ALWAYS, 1, 0xff );
        //         glStencilMask( 0xff );

        glDepthFunc( GL_LEQUAL );
        glDepthMask( GL_FALSE );
        UseShader( skyboxShader );
        glm::mat4 skyboxView = glm::mat4( glm::mat3( GetViewMatrix( &camera ) ) );
        ShaderSetMat4( skyboxShader, "view", glm::value_ptr( skyboxView ) );
        glBindVertexArray( skyboxVAO );
        glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glDepthMask( GL_TRUE );
        glDepthFunc( GL_LESS );
        glDisable( GL_CULL_FACE );

        //         UseShader( modelShader );
        //         glm::mat4 model1 = glm::mat4( 1.0f );
        //         model1 = glm::translate( model1, glm::vec3( 2.0f, 0.0f, 0.0f ) );
        //         ShaderSetMat4( modelShader, "model", glm::value_ptr( model1 ) );
        //         ShaderSetInt( modelShader, "material.texture_specular1", 0 );
        //         DrawModel( grass, modelShader );

        //         model1 = glm::translate( model1, glm::vec3( -2.0f, 0.0f, 2.0f ) );
        //         ShaderSetMat4( modelShader, "model", glm::value_ptr( model1 ) );
        //         DrawModel( redWindow, modelShader );

        //         model1 = glm::translate( model1, glm::vec3( -1.25f, 1.25f, 0.1f ) );
        //         ShaderSetMat4( modelShader, "model", glm::value_ptr( model1 ) );
        //         DrawModel( redWindow, modelShader );

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
