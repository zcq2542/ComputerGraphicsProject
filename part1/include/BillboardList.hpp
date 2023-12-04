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

/**
 * class for trees
 */
class BillboardList{
public:
    BillboardList(std::string fileName);
    ~BillboardList();
    
    // void SetPos(std::vector<float>& pos);
    void SetPos(std::vector<glm::vec2>& vectorList);
    void RandomSetPos(int m, int n); 
    void Initialize();
    void PreDraw();
    void Draw();
private:
    std::vector<float> treePos;
    GLuint mVAO = 0;
    GLuint mVBO[2];
    GLuint mShaderID = 0;
    Texture* mTexture;
    void CreateGraphicsPipeline();
    void VertexSpecification();

};
