#pragma once
#include "utils/utils.h"
#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

global_variable float32 defaultYaw = -90.0f;
global_variable float32 defaultPitch = 0.0f;
global_variable float32 defaultSpeed = 2.5f;
global_variable float32 defaultZoom = 45.0f;
global_variable float32 defaultSensitivity = 0.1f;

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

struct Camera
{
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float32 yaw;
    float32 pitch;

    float32 movementSpeed;
    float32 mouseSensitivity;
    float32 zoom;
};

internal void UpdateCameraVectors( Camera *camera )
{
    glm::vec3 front;
    front.x = cos( glm::radians( camera->yaw ) ) * cos( glm::radians( camera->pitch ) );
    front.y = sin( glm::radians( camera->pitch ) );
    front.z = sin( glm::radians( camera->yaw ) ) * cos( glm::radians( camera->pitch ) );
    camera->front = glm::normalize( front );

    camera->right = glm::normalize( glm::cross( camera->front, camera->worldUp ) );
    camera->up = glm::normalize( glm::cross( camera->right, camera->front ) );
}

internal Camera CreateCamera( glm::vec3 position, glm::vec3 worldUp = glm::vec3( 0.0f, 1.0f, 0.0f ),
                              float32 yaw = defaultYaw, float32 pitch = defaultPitch )
{
    Camera result = {};
    result.position = position;
    result.front = glm::vec3( 0.0f, 0.0f, -1.0f );
    result.worldUp = worldUp;
    result.yaw = yaw;
    result.pitch = pitch;
    result.movementSpeed = defaultSpeed;
    result.mouseSensitivity = defaultSensitivity;
    result.zoom = defaultZoom;
    UpdateCameraVectors( &result );
    return result;
}

inline glm::mat4 GetViewMatrix( Camera *camera )
{
    return glm::lookAt( camera->position, camera->position + camera->front, camera->up );
}

internal void CameraProcessKeyboard( Camera *camera, Camera_Movement direction, float32 deltaTime )
{
    float32 velocity = camera->movementSpeed * deltaTime;
    if ( direction == FORWARD ) { camera->position += camera->front * velocity; }
    if ( direction == BACKWARD ) { camera->position -= camera->front * velocity; }
    if ( direction == LEFT ) { camera->position -= camera->right * velocity; }
    if ( direction == RIGHT ) { camera->position += camera->right * velocity; }
}

internal void CameraProcessMouse( Camera *camera, float32 xOffset, float32 yOffset, GLboolean constrainPitch = true )
{
    xOffset *= camera->mouseSensitivity;
    yOffset *= camera->mouseSensitivity;

    camera->yaw += xOffset;
    camera->pitch += yOffset;

    if ( constrainPitch )
    {
        if ( camera->pitch > 89.0f ) { camera->pitch = 89.0f; }
        if ( camera->pitch < -89.0f ) { camera->pitch = -89.0f; }
    }

    UpdateCameraVectors( camera );
}

internal void CameraProcessScroll( Camera *camera, float32 yOffset )
{
    camera->zoom -= yOffset;
    if ( camera->zoom < 1.0f ) { camera->zoom = 1.0f; }
    if ( camera->zoom > 45.0f ) { camera->zoom = 45.0f; }
}
