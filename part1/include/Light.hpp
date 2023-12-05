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
    glm::vec3 mLightDir;
    int lightOn = 1;


	GLuint mShaderID;	
    GLuint mVAO;
    GLuint mVBO;

    /// Constructor
	Light();

    Light(glm::vec3 position, glm::vec3 lightCol, glm::vec3 lightDir, float spec, float ambient);

    // Initialization function that can be called after
    // OpenGL has been setup
    void Initialize();

    // Operations that happen before drawing
    void PreDraw(glm::vec3 tragetCoord);
	// Draw a light
	void Draw();

};


#endif
