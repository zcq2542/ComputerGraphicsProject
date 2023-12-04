#include "util.hpp"
#include "globals.hpp"
#include "Texture.hpp"
#include "BillboardList.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

BillboardList::BillboardList(std::string fileName){
    mTexture = new Texture();
    mTexture->LoadTexture(fileName);
}

BillboardList::~BillboardList(){
    if(mTexture) delete mTexture;
    glDeleteBuffers(2, mVBO);
    glDeleteVertexArrays(1, &mVAO);

    // Delete our Graphics pipeline
    glDeleteProgram(mShaderID);

}

void BillboardList::SetPos(std::vector<float>& pos){
    this->treePos = pos;
}

void BillboardList::Initialize(){
    BillboardList::CreateGraphicsPipeline();
    // Specify geometry
    BillboardList::VertexSpecification();

}

void BillboardList::CreateGraphicsPipeline(){
    std::string vertexShaderSource      = LoadShaderAsString("./shaders/billboard_vert.glsl");
    std::string geometryShaderSource    = LoadShaderAsString("./shaders/billboard_geom.glsl");
    std::string fragmentShaderSource    = LoadShaderAsString("./shaders/billboard_frag.glsl");

    mShaderID = Create3ShaderProgram(vertexShaderSource,geometryShaderSource, fragmentShaderSource);
    std::cout << "Create mShaderID: " << mShaderID << std::endl;


}

void BillboardList::VertexSpecification(){
        // Vertex Arrays Object (VAO) Setup
    glGenVertexArrays(1, &mVAO);
    // We bind (i.e. select) to the Vertex Array Object (VAO) that we want to work withn.
    glBindVertexArray(mVAO);

    // Vertex Buffer Object (VBO) creation
    glGenBuffers(2, mVBO);

    // Populate our vertex buffer objects
    // Position information (x,y,z)
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, treePos.size() * sizeof(float), treePos.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 1);
    
    // Unbind our currently bound Vertex Array Object
    glBindVertexArray(0);
    // Disable any attributes we opened in our Vertex Attribute Arrray,
    // as we do not want to leave them open.
    glDisableVertexAttribArray(0);
}

void BillboardList::PreDraw(){
    // Use our shader
    glBindVertexArray(mVAO);
    glUseProgram(mShaderID);
    std::cout << "using mShaderID: " << mShaderID << std::endl;
    
    // Update the View Matrix
    GLint u_ViewMatrixLocation = glGetUniformLocation(mShaderID,"u_ViewMatrix");
    if(u_ViewMatrixLocation>=0){
        glm::mat4 viewMatrix = g.gCamera.GetViewMatrix();
        glUniformMatrix4fv(u_ViewMatrixLocation,1,GL_FALSE,&viewMatrix[0][0]);
    }else{
        std::cout << "Could not find u_ViewMatrix, maybe a mispelling?\n";
        //exit(EXIT_FAILURE);
    }

    // Projection matrix (in perspective) 
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
                                             (float)g.gScreenWidth/(float)g.gScreenHeight,
                                             0.1f,
                                             20.0f);

    // Retrieve our location of our perspective matrix uniform 
    GLint u_ProjectionLocation= glGetUniformLocation( mShaderID,"u_Projection");
    if(u_ProjectionLocation>=0){
        glUniformMatrix4fv(u_ProjectionLocation,1,GL_FALSE,&perspective[0][0]);
    }else{
        std::cout << "Could not find u_Perspective, maybe a mispelling?\n";
        //exit(EXIT_FAILURE);
    }

    // Setup view direction
    GLint viewDirectionLocation = glGetUniformLocation(mShaderID, "u_ViewDirection");
    if(viewDirectionLocation >=0){
        glUniform3fv(viewDirectionLocation, 1, &g.gCamera.GetViewDirection()[0]);
    }else{
        std::cout << "Could not find u_ViewDirection" << std::endl;
        //exit(EXIT_FAILURE);
    }

    // Setup eye position
    GLint eyePositionLocation = glGetUniformLocation(mShaderID, "u_EyePosition");
    if(eyePositionLocation >=0){
        glUniform3fv(eyePositionLocation, 1, &g.gCamera.GetEyePosition()[0]);
    }else{
        std::cout << "Could not find u_EyePosition in " << mShaderID << std::endl;
    }

    // Setup head light scope
    GLint headLightScopeLocation = glGetUniformLocation(mShaderID, "u_HeadLightScope");
    if(headLightScopeLocation >=0){
        glUniform1f(headLightScopeLocation, g.gCamera.GetHeadLightScope());
    }else{
        std::cout << "Could not find u_headLightScope" << std::endl;
    }

    // Setup head light on
    GLint headLightOnLocation = glGetUniformLocation(mShaderID, "u_HeadLightOn");
    if(headLightOnLocation >=0){
        //std::cout << "u_HeadLightOn" << g.gCamera.GetIfLightOn() << std::endl;
        glUniform1i(headLightOnLocation, g.gCamera.GetIfLightOn());
    }else{
        std::cout << "Could not find u_headLightOn" << std::endl;
        //exit(EXIT_FAILURE);
    }


    // Setup head light Strength
    GLint headLightStrengthLocation = glGetUniformLocation(mShaderID, "u_HeadLightStrength");
    if(headLightStrengthLocation >=0){
        //std::cout << "u_HeadLightStrength" << g.gCamera.GetLightStrength() << std::endl;
        glUniform1f(headLightStrengthLocation, g.gCamera.GetLightStrength());
    }else{
        std::cout << "Could not find u_headLightStrength" << std::endl;
        //exit(EXIT_FAILURE);
    }


    // Setup head light col
    //glm::vec3 l = g.gCamera.GetHeadLightCol();
    // std::cout << l.x << ", " << l.y << ", " << l.z << std::endl;
    GLint headLightColLocation = glGetUniformLocation(mShaderID, "u_HeadLightCol");
    if(headLightColLocation >=0){
        glUniform3fv(headLightColLocation, 1, &g.gCamera.GetHeadLightCol()[0]);
    }else{
        std::cout << "Could not find u_headLightCol" << std::endl;
    }
    
    
    
    //
    mTexture->Bind(0);
    std::string uniformName = "textureSampler";
    GLint u_diffuseTextureLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
    if(u_diffuseTextureLocation>=0){
        // Setup the slot for the texture
        glUniform1i(u_diffuseTextureLocation,0);
    }else{
        std::cout << "Could not find " << uniformName << std::endl;
        //exit(EXIT_FAILURE);
    }
    
}



/**
* Draw object
*
* @return void
*/
void BillboardList::Draw(){
    // Render data
    glBindVertexArray(mVAO);
    glDrawArraysInstanced(GL_POINTS, 0, 1, treePos.size()/3); // very Instance 4 point
    //glDrawArrays(GL_POINTS, 0, treePos.size());
}

