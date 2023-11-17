#include "OBJ.hpp"
#include "Texture.hpp"
#include "globals.hpp"
#include "util.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

bool hasMTLFile = false;

// Constructor loads a filename with the .ppm extension
OBJ::OBJ(std::string fileName) {
    // open the file 
    std::ifstream inFile;
    inFile.open(fileName);

    // check filepath is correct
    if (!inFile.is_open()) {
        std::cerr << "Could not open file: " << fileName << std::endl;
        std::cerr << "Make sure the path is relative to where you are executing your program" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if(type == "mtllib"){ // if we have mtl file for this obj
            std::filesystem::path filePath = fileName;
            std::string mtlFileName;
            iss >> mtlFileName;
            std::string mtlFilePath = filePath.parent_path().string() + "/" + mtlFileName;
            // load and parse mtl file
            std::cout << "mtl file path: " << mtlFilePath << std::endl;
            // check if success
            hasMTLFile = LoadMTLFile(mtlFilePath);
            // load diffuse texture file if exist 
            if (!mMaterial.diffuseTexture.empty()) {
                std::cout << "parsing diffuse texture" << std::endl;   
                std::string diffuseTextureFile = filePath.parent_path().string() + "/" + mMaterial.diffuseTexture;
                mTextureDiffuse = new Texture();
                mTextureDiffuse->LoadTexture(diffuseTextureFile);
            }
            // load normal texture file if exist 
            if (!mMaterial.normalTexture.empty()) {
                std::cout << "parsing normal texture" << std::endl;   
                std::string normalTextureFile = filePath.parent_path().string() + "/" + mMaterial.normalTexture;
                mTextureNormal = new Texture();
                mTextureNormal->LoadTexture(normalTextureFile);
            }
            // load specular texture file if exist 
            if (!mMaterial.specularTexture.empty()) {
                std::cout << "parsing specular texture" << std::endl;   
                std::string specularTextureFile = filePath.parent_path().string() + "/" + mMaterial.specularTexture;
                mTextureSpecular = new Texture();
                mTextureSpecular->LoadTexture(specularTextureFile);
            }
        }

        if (type == "v") {  
            GLfloat x, y, z;
            iss >> x >> y >> z;
            mVertices.push_back(x);
            mVertices.push_back(y);
            mVertices.push_back(z);
        } 
        if(type == "vt"){
            GLfloat u, v;
            iss >> u >> v;
            mTextureCoord.push_back(u);
            mTextureCoord.push_back(v);
        }
        if (type == "vn") {  
            GLfloat nx, ny, nz;
            iss >> nx >> ny >> nz;
            mNormals.push_back(nx);
            mNormals.push_back(ny);
            mNormals.push_back(nz);
        }
        else if (type == "f") {
            while(iss) {
                std::string part;
                iss >> part;
                std::stringstream issp(part);
                std::string vIdx;
                std::string vtIdx;
                std::string vnIdx;
                if (std::getline(issp, vIdx, '/')) {
                    int v_idx = std::stoi(vIdx) - 1;
                    mVertexIndex.push_back(v_idx);
                    mVerticesArray.push_back(mVertices[v_idx*3]);
                    mVerticesArray.push_back(mVertices[v_idx*3+1]);
                    mVerticesArray.push_back(mVertices[v_idx*3+2]);
                }
                if (std::getline(issp, vtIdx, '/')) {
                    // parse texture if vt exist
                    if (!mTextureCoord.empty()) {
                        int vt_idx = std::stoi(vtIdx) - 1;
                        mTextureArray.push_back(mTextureCoord[vt_idx*2]);
                        mTextureArray.push_back(mTextureCoord[vt_idx*2+1]);
                    }
                }
                if (std::getline(issp, vnIdx, '/')) {
                    int vn_idx = std::stoi(vnIdx) - 1;
                    mNormalsArray.push_back(mNormals[vn_idx*3]);
                    mNormalsArray.push_back(mNormals[vn_idx*3+1]);
                    mNormalsArray.push_back(mNormals[vn_idx*3+2]);
                }
            }
        }
    }
    inFile.close();
}

// Destructor 
OBJ::~OBJ(){
    // Delete our OpenGL Objects
    glDeleteBuffers(5, mVBO);
    glDeleteVertexArrays(1, &mVAO);

    // Delete our Graphics pipeline
    glDeleteProgram(mShaderID);

    if (mTextureDiffuse != nullptr) {
        delete mTextureDiffuse;
    }
    if (mTextureNormal != nullptr) {
        delete mTextureNormal;
    }
    if (mTextureSpecular != nullptr) {
        delete mTextureSpecular;
    }

    OBJ::clear();

}

/**
* Initialize graphics pipeline, generate obejct, and setup geometry
*
* @return void
*/
void OBJ::Initialize() {
    CalculateTB();
    // Create the graphics pipeline
    OBJ::CreateGraphicsPipeline();
    // Specify geometry
    OBJ::VertexSpecification();
}

/**
* PreDraw
* Typically we will use this for setting some sort of 'state'
* Note: some of the calls may take place at different stages (post-processing) of the
* 		 pipeline.
* @return void
*/
void OBJ::PreDraw(glm::vec3 objectCoord){
    // Use our shader
	glUseProgram(mShaderID);

    // Model transformation by translating our object into world space
    // glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(1.0f,0.0f,1.0f)); 
    glm::mat4 model = glm::translate(glm::mat4(1.0f), objectCoord);
    static float rot = 0.0f;
    // rot += 0.1f; // Add a rotation
    model = glm::rotate(model,glm::radians(rot),glm::vec3(0.0f,1.0f,0.0f)); 

    // Retrieve our location of our Model Matrix
    GLint u_ModelMatrixLocation = glGetUniformLocation( mShaderID,"u_ModelMatrix");
    if(u_ModelMatrixLocation >=0){
        glUniformMatrix4fv(u_ModelMatrixLocation,1,GL_FALSE,&model[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }


    // Update the View Matrix
    GLint u_ViewMatrixLocation = glGetUniformLocation(mShaderID,"u_ViewMatrix");
    if(u_ViewMatrixLocation>=0){
        glm::mat4 viewMatrix = g.gCamera.GetViewMatrix();
        glUniformMatrix4fv(u_ViewMatrixLocation,1,GL_FALSE,&viewMatrix[0][0]);
    }else{
        std::cout << "Could not find u_ViewMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }

    // Setup light position
    std::string uniformName = "u_Light[0].lightPos";
    // Get the location of the uniform
    GLint lightPosLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
    if (lightPosLocation >= 0) {
        glUniform3fv(lightPosLocation, 1, &g.gLight.mPosition[0]);
    } else {
        std::cout << "Could not find " << uniformName << std::endl;
        //exit(EXIT_FAILURE);
    }

    // Setup light color
    uniformName = "u_Light[0].lightColor";
    GLint lightColorLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
    if (lightColorLocation >= 0) {
        glUniform3fv(lightColorLocation, 1, &g.gLight.mLightColor[0]);
    } else {
        std::cout << "Could not find " << uniformName << std::endl;
        //exit(EXIT_FAILURE);
    }

    // Setup specular strength    
    uniformName = "u_Light[0].specularStrength";
    GLint specularStrengthLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
    if (specularStrengthLocation >= 0) {
        glUniform1f(specularStrengthLocation, g.gLight.mSpecularStrength);
    } else {
        std::cout << "Could not find " << uniformName << std::endl;
        //exit(EXIT_FAILURE);
    }
    
    // Setup ambient intensity
    uniformName = "u_Light[0].ambientIntensity";
    GLint ambientIntensityLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
    if (ambientIntensityLocation >= 0) {
        glUniform1f(ambientIntensityLocation, g.gLight.mAmbientIntensity);
    } else {
        std::cout << "Could not find " << uniformName << std::endl;
        //exit(EXIT_FAILURE);
    }

    // Setup view direction
    GLint viewDirectionLocation = glGetUniformLocation(mShaderID, "u_ViewDirection");
    if(viewDirectionLocation >=0){
        glUniform3fv(viewDirectionLocation, 1, &g.gCamera.GetViewDirection()[0]);
    }else{
        std::cout << "Could not find u_ViewDirection" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Setup eye position
    GLint eyePositionLocation = glGetUniformLocation(mShaderID, "u_EyePosition");
    if(eyePositionLocation >=0){
        glUniform3fv(eyePositionLocation, 1, &g.gCamera.GetEyePosition()[0]);
    }else{
        std::cout << "Could not find u_EyePosition in " << mShaderID << std::endl;
        //exit(EXIT_FAILURE);
    }
    
    // Setup head light scope
    GLint headLightScopeLocation = glGetUniformLocation(mShaderID, "u_HeadLightScope");
    if(headLightScopeLocation >=0){
        glUniform1f(headLightScopeLocation, g.gCamera.GetHeadLightScope());
    }else{
        std::cout << "Could not find u_headLightScope" << std::endl;
        //exit(EXIT_FAILURE);
    }

    // Setup head light col
    glm::vec3 l = g.gCamera.GetHeadLightCol();
    std::cout << l.x << ", " << l.y << ", " << l.z << std::endl;
    GLint headLightColLocation = glGetUniformLocation(mShaderID, "u_HeadLightCol");
    if(headLightColLocation >=0){
        glUniform3fv(headLightColLocation, 1, &g.gCamera.GetHeadLightCol()[0]);
    }else{
        std::cout << "Could not find u_headLightCol" << std::endl;
        //exit(EXIT_FAILURE);
    }



    // Setup shininess
    uniformName = "u_Material.shininess";
    GLint shininessLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
    if (shininessLocation >= 0) {
        // if material shininess exist and not equal to 0.0, we use material texture's shininess
        if (mMaterial.shininess != -1.0 && mMaterial.shininess != 0.0) {
            glUniform1f(shininessLocation, mMaterial.shininess);
        } else {
            // else set a default 32 shininess 
            glUniform1f(shininessLocation, 32);
        }
    } else {
        std::cout << "Could not find " << uniformName << std::endl;
        exit(EXIT_FAILURE);
    }

    // Setup object ambient color
    uniformName = "u_Material.ka";
    GLint ambientColorLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
    if (ambientColorLocation >= 0) {
        // if material ambient color exist and is not too dark
        if (hasMTLFile && mMaterial.ambient.r >= 0.5f && mMaterial.ambient.g >= 0.5f && mMaterial.ambient.b >= 0.5f) {
            glUniform3fv(ambientColorLocation, 1, &mMaterial.ambient[0]);
        } else {
            // else set a default u_Ka
            glUniform3fv(ambientColorLocation, 1, &glm::vec3(1.f, 1.f, 1.f)[0]);
        }
    } else {
        std::cout << "Could not find " << uniformName << std::endl;
        exit(EXIT_FAILURE);
    }

    // Setup object diffuse color
    uniformName = "u_Material.kd";
    GLint diffuseColorLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
    if (diffuseColorLocation >= 0) {
        // if material diffuse color exist and is not too dark
        if (hasMTLFile && mMaterial.diffuse.r >= 0.5f && mMaterial.diffuse.g >= 0.5f && mMaterial.diffuse.b >= 0.5f) {
            glUniform3fv(diffuseColorLocation, 1, &mMaterial.diffuse[0]);
        } else {
            // else set a default u_Ka
            glUniform3fv(diffuseColorLocation, 1, &glm::vec3(1.f, 1.f, 1.f)[0]);
        }
    } else {
        std::cout << "Could not find " << uniformName << std::endl;
        exit(EXIT_FAILURE);
    }

    // Setup object specular color
    uniformName = "u_Material.ks";
    GLint specularColorLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
    if (specularColorLocation >= 0) {
        // if material specular color exist and is not too dark
        if (hasMTLFile && mMaterial.specular.r >= 0.5f && mMaterial.specular.g >= 0.5f && mMaterial.specular.b >= 0.5f) {
            glUniform3fv(specularColorLocation, 1, &mMaterial.specular[0]);
        } else {
            // else set a default u_Ka
            glUniform3fv(specularColorLocation, 1, &glm::vec3(1.f, 1.f, 1.f)[0]);
        }
    } else {
        std::cout << "Could not find " << uniformName << std::endl;
        exit(EXIT_FAILURE);
    }

    // Setup boolean uniform if object has specular texture
    uniformName = "u_HasSpecularTexture";
    GLint hasSpecularLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
    if (hasSpecularLocation >= 0) {
        // if TextureCoords exist
        if (!mMaterial.specularTexture.empty()) {
            glUniform1i(hasSpecularLocation, true);
        } else {
            // else set to false
            glUniform1i(hasSpecularLocation, false);
        }
    } else {
        std::cout << "Could not find " << uniformName << std::endl;
        exit(EXIT_FAILURE);
    }

    // Bind diffuse texture
    if (!mMaterial.diffuseTexture.empty()) {
        // Bind our diffuse texture to slot number 0
        mTextureDiffuse->Bind(0);

        // Setup diffuse texture uniform 
        uniformName = "u_Material.diffuseTexture";
        GLint u_diffuseTextureLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
        if(u_diffuseTextureLocation>=0){
            // Setup the slot for the texture
            glUniform1i(u_diffuseTextureLocation,0);
        }else{
            std::cout << "Could not find" << uniformName << std::endl;
        exit(EXIT_FAILURE);
        }
    }

    // Bind normal texture
    if (!mMaterial.normalTexture.empty()) {
        // Bind our normal texture to slot number 1
        mTextureNormal->Bind(1);

        // Setup normal texture uniform 
        uniformName = "u_Material.normalTexture";
        GLint u_normalTextureLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
        if(u_normalTextureLocation>=0){
            // Setup the slot for the texture
            glUniform1i(u_normalTextureLocation,1);
        }else{
            std::cout << "Could not find" << uniformName << std::endl;
        exit(EXIT_FAILURE);
        }
    }

    // Bind specular texture
    if (!mMaterial.specularTexture.empty()) {
        // Bind our specular texture to slot number 1
        mTextureSpecular->Bind(2);

        // Setup specular texture uniform 
        uniformName = "u_Material.specularTexture";
        GLint u_specularTextureLocation = glGetUniformLocation(mShaderID, uniformName.c_str());
        if(u_specularTextureLocation>=0){
            // Setup the slot for the texture
            glUniform1i(u_specularTextureLocation,2);
        }else{
            std::cout << "Could not find" << uniformName << std::endl;
        exit(EXIT_FAILURE);
        }
    }
}

/**
* Draw object
*
* @return void
*/
void OBJ::Draw(){
    // Render data
	glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, mVerticesArray.size()/3);
}

/**
* Create the graphics pipeline
*
* @return void
*/
void OBJ::CreateGraphicsPipeline() {
    std::string vertexShaderSource      = LoadShaderAsString("./shaders/vert.glsl");
    std::string fragmentShaderSource    = LoadShaderAsString("./shaders/frag.glsl");

    mShaderID = CreateShaderProgram(vertexShaderSource,fragmentShaderSource);
}

/**
* Setup geometry during the vertex specification step
*
* @return void
*/
void OBJ::VertexSpecification() {
    // Vertex Arrays Object (VAO) Setup
    glGenVertexArrays(1, &mVAO);
    // We bind (i.e. select) to the Vertex Array Object (VAO) that we want to work withn.
    glBindVertexArray(mVAO);

    // Vertex Buffer Object (VBO) creation
    glGenBuffers(5, mVBO);

    // Populate our vertex buffer objects
    // Position information (x,y,z)
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, mVerticesArray.size() * sizeof(float), mVerticesArray.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, mNormalsArray.size() * sizeof(float), mNormalsArray.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Texture information 
    if (!mTextureArray.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, mVBO[2]);
        glBufferData(GL_ARRAY_BUFFER, mTextureArray.size() * sizeof(float), mTextureArray.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // vertex tangent
    if (!mTangentArray.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, mVBO[3]);
        glBufferData(GL_ARRAY_BUFFER, mTangentArray.size() * sizeof(float), mTangentArray.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // vertex bitangent
    if (!mBitangentArray.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, mVBO[4]);
        glBufferData(GL_ARRAY_BUFFER, mBitangentArray.size() * sizeof(float), mBitangentArray.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // Unbind our currently bound Vertex Array Object
    glBindVertexArray(0);
    // Disable any attributes we opened in our Vertex Attribute Arrray,
    // as we do not want to leave them open. 
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
}

/**
 * Load mtl file, 
 * if load successful return 1, otherwise return 0
 * @return int
*/
int OBJ::LoadMTLFile(std::string mtlFileName) {
    std::ifstream mtlFile(mtlFileName);
    if (!mtlFile.is_open()) {
        std::cerr << "Could not open MTL file: " << mtlFileName << std::endl;
        std::cerr << "Make sure the path is relative to where you are executing your program" << std::endl;
        return 0;
    }

    std::string line;
    while (std::getline(mtlFile, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "newmtl") {
            iss >> mMaterial.name;
        } else if (type == "Kd") {
            iss >> mMaterial.diffuse.r >> mMaterial.diffuse.g >> mMaterial.diffuse.b;
        } else if (type == "Ka") {
            iss >> mMaterial.ambient.r >> mMaterial.ambient.g >> mMaterial.ambient.b;
        } else if (type == "Ks") {
            iss >> mMaterial.specular.r >> mMaterial.specular.g >> mMaterial.specular.b;
        } else if (type == "Ns") {
            iss >> mMaterial.shininess;
        } else if (type == "map_Kd") {
            iss >> mMaterial.diffuseTexture;
        } else if (type == "map_Bump") {
            iss >> mMaterial.normalTexture;
        } else if (type == "map_Ks") {
            iss >> mMaterial.specularTexture;
        }
    }

    mtlFile.close();
    return 1;
}

void OBJ::CalculateTB(){
    glm::vec3 tangent;
    glm::vec3 bitangent;
    for(int i = 0; i < mVertexIndex.size(); i+=3){
        int vert0 = i;
        int vert1 = i+1;
        int vert2 = i+2;

	    // Look up the actual vertex positions
        glm::vec3 pos0(mVerticesArray[vert0*3 +0], mVerticesArray[vert0*3 + 1], mVerticesArray[vert0*3 + 2]); 
        glm::vec3 pos1(mVerticesArray[vert1*3 +0], mVerticesArray[vert1*3 + 1], mVerticesArray[vert1*3 + 2]); 
        glm::vec3 pos2(mVerticesArray[vert2*3 +0], mVerticesArray[vert2*3 + 1], mVerticesArray[vert2*3 + 2]); 

	    // Look up the texture coordinates
        glm::vec2 tex0(mTextureArray[vert0*2 +0], mTextureArray[vert0*2 + 1]);
        glm::vec2 tex1(mTextureArray[vert1*2 +0], mTextureArray[vert1*2 + 1]);
        glm::vec2 tex2(mTextureArray[vert2*2 +0], mTextureArray[vert2*2 + 1]);

        // create edges
        glm::vec3 edge0 = pos1 - pos0;
        glm::vec3 edge1 = pos2 - pos0;
    
        glm::vec2 deltaUV0 = tex1-tex0;
        glm::vec2 deltaUV1 = tex2-tex0;

        float f = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

        tangent.x = f * (deltaUV1.y * edge0.x - deltaUV0.y* edge1.x);
        tangent.y = f * (deltaUV1.y * edge0.y - deltaUV0.y* edge1.y);
        tangent.z = f * (deltaUV1.y * edge0.z - deltaUV0.y* edge1.z);
        tangent = glm::normalize(tangent);

        bitangent.x = f * (-deltaUV1.x * edge0.x + deltaUV0.x* edge1.x);
        bitangent.y = f * (-deltaUV1.x * edge0.y + deltaUV0.x* edge1.y);
        bitangent.z = f * (-deltaUV1.x * edge0.z + deltaUV0.x* edge1.z);
        bitangent = glm::normalize(bitangent);

        // Compute a tangent
        for(int k = 0; k < 3; ++k){
            mTangentArray.push_back(tangent.x); 
            mTangentArray.push_back(tangent.y);
            mTangentArray.push_back(tangent.z);
        }

        // Compute a bi-tangent
        for(int k = 0; k < 3; ++k){
            mBitangentArray.push_back(bitangent.x); 
            mBitangentArray.push_back(bitangent.y); 
            mBitangentArray.push_back(bitangent.z);
        }    
    }
}

/**
* Clear vectors
*
* @return void
*/
void OBJ::clear() {
    mVertices.clear();
    mNormals.clear();
    mTextureCoord.clear();
    mVerticesArray.clear();
    mNormalsArray.clear();
    mTextureArray.clear();
    mTangentArray.clear();
    mBitangentArray.clear();
}
