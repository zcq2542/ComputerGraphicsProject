#include "Camera.hpp"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <algorithm>
#include <SDL2/SDL.h>
#include <stdio.h>

#include <iostream>

void Camera::MouseLook(int mouseX, int mouseY){
    // set limit for mouseY
    mouseY = std::clamp(mouseY, -50, 600);

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

glm::vec3 Camera::CheckForward(float speed){
    return glm::vec3(m_eyePosition.x + speed * m_viewDirection.x,
                    m_eyePosition.y,
                    m_eyePosition.z + speed * m_viewDirection.z);
}

glm::vec3 Camera::CheckBackward(float speed){
    return glm::vec3(m_eyePosition.x - speed * m_viewDirection.x,
                    m_eyePosition.y,
                    m_eyePosition.z - speed * m_viewDirection.z);
}

glm::vec3 Camera::CheckLeft(float speed){
    // Compute the right vector
    glm::vec3 rightVector = glm::normalize(glm::cross(m_viewDirection, m_upVector));
    return glm::vec3(m_eyePosition - speed * rightVector);
}

glm::vec3 Camera::CheckRight(float speed){
    // Compute the right vector
    glm::vec3 rightVector = glm::normalize(glm::cross(m_viewDirection, m_upVector));
    return glm::vec3(m_eyePosition + speed * rightVector);
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
    if(HeadLightOn == 1){
        if(SDL_GetTicks() < ShutDownTime){
            BatteryTime = (int)(ShutDownTime - SDL_GetTicks())/1000;
        }
        if(SDL_GetTicks() > ShutDownTime){
            SwitchLight();
            std::cout << "Out of battery!" << std::endl;
        }
        else if(ShutDownTime - SDL_GetTicks() < 15000 && SDL_GetTicks() > RecoverTime ){
            int rd = rand() % 100;
            if(rd > 60){ 
                SwitchLight();
            }
        }
        else if(ShutDownTime - SDL_GetTicks() > 15000){
            LightStrength = 1.0f;
        }
    }
    else if(BatteryTime > 0){ // HeadLightOn == 0
        if(BatteryTime < 15){
            int min = 200;
            int max = 1200;
            int randomNumber = min + rand() % (max - min + 1);
            LightStrength = BatteryTime / 15.0f;
            // std::cout << "Flashlight Strength: " << LightStrength << std::endl;
            RecoverTime = SDL_GetTicks() + randomNumber; // current to recovertime light is on.
            SwitchLight();
        }
        else{
            std::cout << "Battery refilled!" << std::endl;
            LightStrength = 1;
            SwitchLight();
            CountdownNum = 10;
        }
    }
    if(SDL_GetTicks() > ShutDownTime){
        int nowNum = (int)(ShutDownTime + 11000 - SDL_GetTicks()) /1000;
        if(CountdownNum > nowNum) { 
            CountdownNum = nowNum;
            std::cout << "count down: " << CountdownNum<< std::endl;
        }
    }
    if(SDL_GetTicks() > ShutDownTime + 10000){
        std::cout << "==========Game Over===========" << std::endl;
        GameOver = true;
    }
}

void Camera::SwitchLight(){
    if(HeadLightOn == 1){
        BatteryTime = (ShutDownTime - SDL_GetTicks()) / 1000; // ms to s
        HeadLightOn = 0;
        // std::cout << "turn off" << std::endl;
    }

    else if(HeadLightOn == 0 && BatteryTime > 0){
        ShutDownTime = SDL_GetTicks()+BatteryTime * 1000; // s to ms
        HeadLightOn = 1;
        // std::cout << "turn on" << std::endl;
    }
}

int Camera::GetIfLightOn(){
    return HeadLightOn;
}

float Camera::GetLightStrength(){
    return LightStrength;
}

void Camera::CollectBattery(){
    if(BatteryTime > 0){
        BatteryTime += 30;
        ShutDownTime += 30 * 1000;
    }
    else {
        BatteryTime = 30;
        ShutDownTime = SDL_GetTicks() + 30 * 1000;
    }
}

void Camera::GetBatteryInfo(){
    std::cout << "BatteryTime: " << BatteryTime << std::endl;
    // std::cout << "ShutDownTime: " << ShutDownTime << std::endl;
    // std::cout << "HeadLightOn: " << HeadLightOn << std::endl;
    // std::cout << "HeadLightStrength: " << LightStrength << std::endl;

}

bool Camera::GetGameOver(){
    return GameOver;
}

Camera::Camera(){
    // std::cout << "Camera.cpp: (Constructor) Created a Camera!\n";
	// Position us at the origin.
    m_eyePosition = glm::vec3(0.0f, m_cameraYCoord, 0.0f);
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
    BatteryTime = 70;
    ShutDownTime = SDL_GetTicks() + BatteryTime * 1000; // battery time is 70s.
    RecoverTime = 0;
    LightStrength = 1.0f;

}

glm::mat4 Camera::GetViewMatrix() const{
    // Think about the second argument and why that is
    // setup as it is.
    return glm::lookAt( m_eyePosition,
                        m_eyePosition + m_viewDirection,
                        m_upVector);
}
