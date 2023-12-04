/** @file OBJ.hpp
 *  @brief Class for working with OBJ object
 *  
 *  Class for parsing OBJ object specifically.
 *
 *  @author Lingxin Ma
 *  @bug No known bugs.
 */
#ifndef OBJ_HPP
#define OBJ_HPP

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>

#include "util.hpp"
#include "globals.hpp"
#include "Texture.hpp"

class OBJ{
public:
    // Default constructor
    OBJ() { };
    // Constructor loads a filename with the .OBJ extension
    OBJ(std::string fileName);
    // Destructor 
    ~OBJ();

    // Render objects
    void Initialize();
    void PreDraw(glm::vec3 objectCoord, float rot = 0.0f);
    void Draw();

    // Get vertex and normal data
    inline std::vector<GLfloat> getVerticesArray() const { return mVerticesArray; }
    inline std::vector<GLfloat> getNormalsArray() const { return mNormalsArray; }

    // Get texture data
    inline std::vector<GLfloat> getTextureArray() const { return mTextureArray; }

    // Get min coordinate 
    inline glm::vec3 getMinCoord() const { return mMin; }
    // Get max coordinate 
    inline glm::vec3 getMaxCoord() const { return mMax; }
    // Get object coordinate
    inline glm::vec3 getObjectCoord() const { return mObjectCoord; }
    // Set object coordinate
    inline void setObjectCoord(glm::vec3 coord) { mObjectCoord = coord; }
    // Get object rotation
    inline float getRot() const { return mRot; }
    // gen rand x, z for battery
    void randomXZCoord(int min, int max);

private:    
    std::vector<GLfloat> mVertexIndex;
    std::vector<GLfloat> mVertices;
    std::vector<GLfloat> mNormals;  
    std::vector<GLfloat> mTextureCoord;
    std::vector<GLfloat> mVerticesArray;
    std::vector<GLfloat> mNormalsArray;
    std::vector<GLfloat> mTextureArray;
    std::vector<GLfloat> mTangentArray;
    std::vector<GLfloat> mBitangentArray;

    // struct to hold material properties and textures
    struct Material {
        std::string name;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
        std::string diffuseTexture;
        std::string normalTexture;
        std::string specularTexture;
    };
    
    Material mMaterial;

    GLuint mVAO = 0;
    GLuint mVBO[5];
    GLuint mShaderID = 0;

    Texture* mTextureDiffuse;
    Texture* mTextureNormal;
    Texture* mTextureSpecular;

    void clear();

    void CreateGraphicsPipeline();
    void VertexSpecification();
    int LoadMTLFile(std::string mtlFileName);
    void CalculateTB();

    glm::vec3 mMin = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);      // Minimum (x, y, z) coordinates
    glm::vec3 mMax = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);   // Maximum (x, y, z) coordinates
    glm::vec3 mObjectCoord = glm::vec3(0, 0, 0);  // origin of object being placed
    float mRot;              // angle rotated along y-axis when being placed 

    bool mDrawGrass = false;            // check if we are drawing grass
    glm::vec3 mTranslations[400];
};

#endif
