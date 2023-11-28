
/** @file Camera.hpp
 *  @brief Sets up an OpenGL camera.
 *  
 *  Sets up an OpenGL Camera. The camera is what
 *  sets up our 'view' matrix.
 *
 *  @author Mike
 *  @bug No known bugs.
 */
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"
#include "Light.hpp"
#include <SDL2/SDL.h>

class Camera{
public:
	// Constructor to create a camera
    Camera();
    // Return a 'view' matrix with our
    // camera transformation applied.
    glm::mat4 GetViewMatrix() const;
    // Move the camera around
    void MouseLook(int mouseX, int mouseY);
    void MoveForward(float speed);
    void MoveBackward(float speed);
    void MoveLeft(float speed);
    void MoveRight(float speed);
    void WalkCycle(float speed);
    void MoveUp(float speed);
    void MoveDown(float speed);
    // Check camera position IF moving in that direction
    glm::vec3 CheckForward(float speed);
    glm::vec3 CheckBackward(float speed);
    glm::vec3 CheckLeft(float speed);
    glm::vec3 CheckRight(float speed);
    // Set the position for the camera
    void SetCameraEyePosition(float x, float y, float z);
    // Set view direction
    void SetViewDirection(float x, float y, float z);
    // Returns the Camera X Position where the eye is 
    float GetEyeXPosition();
    // Returns the Camera Y Position where the eye is 
    float GetEyeYPosition();
    // Returns the Camera Z Position where the eye is 
    float GetEyeZPosition();
	// Returns the X 'view' direction
    float GetViewXDirection();
    // Returns the Y 'view' direction
    float GetViewYDirection();
    // Returns the Z 'view' direction
    float GetViewZDirection();
    // Returns the view direction
    glm::vec3 GetViewDirection();
    // Returns the eye position
    glm::vec3 GetEyePosition();
    // Returns the initla eye postion
    glm::vec3 GetEyeInitialPosition(); 
    // head light scope
    float GetHeadLightScope();
    // head light col
    glm::vec3 GetHeadLightCol();
    void CheckBattery();
    void SwitchLight();
    int GetIfLightOn();
    float GetLightStrength();
private:

    // Track the old mouse position
    glm::vec2 m_oldMousePosition;
    // Where is our camera positioned
    glm::vec3 m_eyePosition;
    // initial position for camera, used to reset view
    glm::vec3 m_eyeInitialPosition;
    // What direction is the camera looking
    glm::vec3 m_viewDirection;
    // Which direction is 'up' in our world
    // Generally this is constant, but if you wanted
    // to 'rock' or 'rattle' the camera you might play
    // with modifying this value.
    glm::vec3 m_upVector;
    // initial height of camera position 
    float m_cameraYCoord = 0.35f; 
    // max height for walk cycle
    float m_walkCycleMaxHeight = 0.03f;
    float light_scope;
    glm::vec3 headLightCol;
    //Light m_headLight;
    int HeadLightOn;
    Uint32 ShutDownTime; //= SDL_GetTicks() + 70 * 1000; // batery time is 60s.
    Uint32 RecoverTime;// = 0;
    int BatteryTime;// = 70;
    float LightStrength;
};




#endif
