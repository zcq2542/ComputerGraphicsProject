#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include "util.hpp"

struct Light{
    glm::vec3 mPosition;
    glm::vec3 mLightColor;
    float mSpecularStrength;
    float mAmbientIntensity;

	GLuint mShaderID;	
    GLuint mVAO;
    GLuint mVBO;

    /// Constructor
	Light();

    // Initialization function that can be called after
    // OpenGL has been setup
    void Initialize();

    // Operations that happen before drawing
    void PreDraw();
	// Draw a light
	void Draw();

};


#endif
