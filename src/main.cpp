#include <stdio.h>
#include <glad/glad.h>
#include <glfw3.h>
#include "utils/utils.h"

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

    GLFWwindow *window = glfwCreateWindow( 1280, 720, "OpenGL Engine", 0, 0 );
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

    glViewport( 0, 0, 1280, 720 );
    glfwSetFramebufferSizeCallback( window, FramebufferSizeCallback );

    float vertices[] = { 0.5f, 0.5f, 0.0f,    // top right
                         0.5f, -0.5f, 0.0f,   // bottom right
                         -0.5f, -0.5f, 0.0f,  // bottom left
                         -0.5f, 0.5f, 0.0f }; // top left
    u32 indices[] = { 0, 1, 3,
                      1, 2, 3 };

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

    char *vertexShaderSource = "#version 330 core\n"
                               "layout (location = 0) in vec3 position;\n"
                               "void main()\n"
                               "{\n"
                               "    gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
                               "}\0";
    u32 vertexShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertexShader, 1, &vertexShaderSource, 0 );
    glCompileShader( vertexShader );
    int success;
    char infoLog[ 512 ];
    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &success );

    if ( !success )
    {
        glGetShaderInfoLog( vertexShader, 512, 0, infoLog );
        printf( "Error during vertex shader compilation:\n%s", infoLog );
    }

    char *fragmentShaderSource = "#version 330 core\n"
                                 "out vec4 fragColor;\n"
                                 "void main()\n"
                                 "{\n"
                                 "    fragColor = vec4(0.3, 0.2, 0.5, 1.0);\n"
                                 "}\0";

    u32 fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragmentShader, 1, &fragmentShaderSource, 0 );
    glCompileShader( fragmentShader );
    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &success );

    if ( !success )
    {
        glGetShaderInfoLog( fragmentShader, 512, 0, infoLog );
        printf( "Error during fragment shader compilation:\n%s", infoLog );
    }

    u32 shaderProgram = glCreateProgram();
    glAttachShader( shaderProgram, vertexShader );
    glAttachShader( shaderProgram, fragmentShader );
    glLinkProgram( shaderProgram );
    glGetProgramiv( shaderProgram, GL_LINK_STATUS, &success );

    if ( !success )
    {
        glGetProgramInfoLog( shaderProgram, 512, 0, infoLog );
        printf( "Error during shader linking:\n%s", infoLog );
    }

    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), 0 );
    glEnableVertexAttribArray( 0 );

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    while ( !glfwWindowShouldClose( window ) )
    {
        ProcessInput( window );

        glClearColor( 0.4f, 0.4f, 0.4f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );

        glfwPollEvents();

        glUseProgram( shaderProgram );
        glBindVertexArray( vao );
        // glDrawArrays( GL_TRIANGLES, 0, 3 );
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
        glBindVertexArray( 0 );

        glfwSwapBuffers( window );
    }

    glfwTerminate();
    return 0;
}
