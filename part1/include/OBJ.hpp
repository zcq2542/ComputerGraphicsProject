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
    void PreDraw(glm::vec3 objectCoord);
    void Draw();

    // Get vertex and normal data
    inline std::vector<GLfloat> getVerticesArray() const { return mVerticesArray; }
    inline std::vector<GLfloat> getNormalsArray() const { return mNormalsArray; }

    // Get texture data
    inline std::vector<GLfloat> getTextureArray() const { return mTextureArray; }

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
};

#endif
