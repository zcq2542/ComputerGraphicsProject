#include "Camera.hpp"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <SDL2/SDL.h>
#include <stdio.h>

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

// Set the view direction
void Camera::SetViewDirection(float x, float y, float z) {
    m_viewDirection.x = x;
    m_viewDirection.y = y;
    m_viewDirection.z = z;
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

glm::vec3 Camera::GetEyePosition() {
    return m_eyePosition;
}

glm::vec3 Camera::GetEyeInitialPosition() {
    return m_eyeInitialPosition;
}

float Camera::GetHeadLightScope() {
    return light_scope; 
}

glm::vec3 Camera::GetHeadLightCol() {
    return headLightCol;
}

void Camera::CheckBattery(){
    //std::cout << "HeadLightOn: " << HeadLightOn << std::endl;
    if(HeadLightOn == 1){
        if(SDL_GetTicks() < ShutDownTime){
            BatteryTime = ShutDownTime - SDL_GetTicks();
            //std::cout << "BatteryTime: " << BatteryTime << std::endl;
            LightStrength = BatteryTime / 15000.0f;
            //std::cout << "LightStrength: " << LightStrength << std::endl;
        }
        if(SDL_GetTicks() > ShutDownTime){
            SwitchLight();
            std::cout << "out of battery" << std::endl;
        }
        else if(ShutDownTime - SDL_GetTicks() < 15000 && SDL_GetTicks() > RecoverTime ){
            int rd = rand() % 100;
            if(rd > 10){ // prob turn off light.
                SwitchLight(); 
            }
        }
    }
    else if(BatteryTime > 0){
        int min = 100;
        int max = 300;
        int randomNumber = min + rand() % (max - min + 1);
        //std::cout << "randomNuber: " << randomNumber << std::endl;
        int rd = rand() % 100;
        if(rd < 30){
            randomNumber += 1000 + rand() % (1000);
        }
        RecoverTime = SDL_GetTicks() + randomNumber; // current to recovertime light is on.
        SwitchLight();
    }
}

void Camera::SwitchLight(){
    if(HeadLightOn == 1){
        BatteryTime = ShutDownTime - SDL_GetTicks();
        HeadLightOn = 0;
        //std::cout << "turn off" << std::endl;
    }

    else if(HeadLightOn == 0 && BatteryTime > 0){
        ShutDownTime = SDL_GetTicks()+BatteryTime;
        HeadLightOn = 1;
        //std::cout << "turn on" << std::endl;
    }
}

int Camera::GetIfLightOn(){
    return HeadLightOn;
}

float Camera::GetLightStrength(){
    return LightStrength;
}

Camera::Camera(){
    std::cout << "Camera.cpp: (Constructor) Created a Camera!\n";
	// Position us at the origin.
    m_eyePosition = glm::vec3(0.0f, m_cameraYCoord, 6.0f);
    m_eyeInitialPosition = m_eyePosition;
	// Looking down along the z-axis initially.
	// Remember, this is negative because we are looking 'into' the scene.
    m_viewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	// For now--our upVector always points up along the y-axis
    m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    headLightCol = glm::vec3(0.8f, 0.8f, 0.8f);
    light_scope = 0.1 * M_PI;
    //m_headLight = Light(m_eyePosition, headLightCol, m_viewDirection, 0.8f, 0.0f);
    HeadLightOn = 1;
    RecoverTime = 0;
    BatteryTime = 30;
    ShutDownTime = SDL_GetTicks() + BatteryTime * 1000; // batery time is 70s.
    LightStrength = 1.0f;

}

glm::mat4 Camera::GetViewMatrix() const{
    // Think about the second argument and why that is
    // setup as it is.
    return glm::lookAt( m_eyePosition,
                        m_eyePosition + m_viewDirection,
                        m_upVector);
}
