#include "Camera.hpp"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <SDL2/SDL.h>

#include <iostream>

// TODO: 
// 1. mouse up and down rotation limit 
void Camera::MouseLook(int mouseX, int mouseY){
    // Record our new position as a vector
    glm::vec2 newMousePosition(mouseX, mouseY);

    // Little hack for our 'mouse look function'
    // We need this so that we can move our camera
    // for the first time.
    static bool firstLook=true;
    if(true == firstLook){
        firstLook=false;
        m_oldMousePosition = newMousePosition;
    }

    // Detect how much the mouse has moved since
    // the last time
    glm::vec2 mouseDelta = m_oldMousePosition - newMousePosition;
    mouseDelta.x *= 0.2f; // mouse sensitivity.
    mouseDelta.y *= 0.2f; // mouse sensitivity.

    // Rotate about the upVector
    m_viewDirection = glm::rotate(m_viewDirection, glm::radians(mouseDelta.x), m_upVector);
    
    // Calculate right vector using cross product
    glm::vec3 rightVector = glm::cross(m_viewDirection, m_upVector);

    // Rotate about the rightVector (reversed pitch, so that when mouse move left, object appears over left)
    m_viewDirection = glm::rotate(m_viewDirection, glm::radians(mouseDelta.y), rightVector);

    // Normalize the view direction
    m_viewDirection = glm::normalize(m_viewDirection);

    // Update our old position after we have made changes 
    m_oldMousePosition = newMousePosition;
}

void Camera::MoveForward(float speed){
    // update m_eyePosition by speed * m_viewDirection
    m_eyePosition.x += speed * m_viewDirection.x;
    m_eyePosition.z += speed * m_viewDirection.z;
    WalkCycle(speed);
}

void Camera::MoveBackward(float speed){
    // update m_eyePosition by -speed * m_viewDirection
    m_eyePosition.x -= speed * m_viewDirection.x;
    m_eyePosition.z -= speed * m_viewDirection.z;
    WalkCycle(speed);
}

void Camera::MoveLeft(float speed){
    // Compute the right vector and update m_eyePosition accordingly
    glm::vec3 rightVector = glm::normalize(glm::cross(m_viewDirection, m_upVector));
    m_eyePosition -= speed * rightVector;
    WalkCycle(speed);
}

void Camera::MoveRight(float speed){
    // Compute the right vector and update m_eyePosition accordingly
    glm::vec3 rightVector = glm::normalize(glm::cross(m_viewDirection, m_upVector));
    m_eyePosition += speed * rightVector;
    WalkCycle(speed);
}

void Camera::WalkCycle(float speed) {
    // Frequency and amplitude for the walking effect (move up and down)
    float verticalOffset = m_walkCycleMaxHeight * sin(SDL_GetTicks() * speed * 0.35f); 
    // Adjust the initial height and add the vertical offset
    m_eyePosition.y = m_cameraYCoord + verticalOffset; 
}

void Camera::MoveUp(float speed){
    m_eyePosition.y += speed;
}

void Camera::MoveDown(float speed){
    m_eyePosition.y -= speed;
}

// Set the position for the camera
void Camera::SetCameraEyePosition(float x, float y, float z){
    m_eyePosition.x = x;
    m_eyePosition.y = y;
    m_eyePosition.z = z;
}

float Camera::GetEyeXPosition(){
    return m_eyePosition.x;
}

float Camera::GetEyeYPosition(){
    return m_eyePosition.y;
}

float Camera::GetEyeZPosition(){
    return m_eyePosition.z;
}

float Camera::GetViewXDirection(){
    return m_viewDirection.x;
}

float Camera::GetViewYDirection(){
    return m_viewDirection.y;
}

float Camera::GetViewZDirection(){
    return m_viewDirection.z;
}

glm::vec3 Camera::GetViewDirection() {
    return m_viewDirection;
}


Camera::Camera(){
    std::cout << "Camera.cpp: (Constructor) Created a Camera!\n";
	// Position us at the origin.
    m_eyePosition = glm::vec3(0.0f, m_cameraYCoord, 6.0f);
	// Looking down along the z-axis initially.
	// Remember, this is negative because we are looking 'into' the scene.
    m_viewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	// For now--our upVector always points up along the y-axis
    m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::GetViewMatrix() const{
    // Think about the second argument and why that is
    // setup as it is.
    return glm::lookAt( m_eyePosition,
                        m_eyePosition + m_viewDirection,
                        m_upVector);
}
