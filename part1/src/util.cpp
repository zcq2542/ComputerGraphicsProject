#include "util.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <random>
#include <glm/glm.hpp>
#include <glad/glad.h>

// vvvvvvvvvvvvvvvvvvv Error Handling Routines vvvvvvvvvvvvvvv
void GLClearAllErrors(){
    while(glGetError() != GL_NO_ERROR){
    }
}

// Returns true if we have an error
bool GLCheckErrorStatus(const char* function, int line){
    while(GLenum error = glGetError()){
        std::cout << "OpenGL Error:" << error 
                  << "\tLine: " << line 
                  << "\tfunction: " << function << std::endl;
        return true;
    }
    return false;
}

#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x,__LINE__);
// ^^^^^^^^^^^^^^^^^^^ Error Handling Routines ^^^^^^^^^^^^^^^



/**
* LoadShaderAsString takes a filepath as an argument and will read line by line a file and return a string that is meant to be compiled at runtime for a vertex, fragment, geometry, tesselation, or compute shader.
* e.g.
*       LoadShaderAsString("./shaders/filepath");
*
* @param filename Path to the shader file
* @return Entire file stored as a single string 
*/
std::string LoadShaderAsString(const std::string& filename){
    // Resulting shader program loaded as a single string
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filename.c_str());

    if(myFile.is_open()){
        while(std::getline(myFile, line)){
            result += line + '\n';
        }
        myFile.close();

    }

    return result;
}


/**
* CompileShader will compile any valid vertex, fragment, geometry, tesselation, or compute shader.
* e.g.
*	    Compile a vertex shader: 	CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
*       Compile a fragment shader: 	CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
*
* @param type We use the 'type' field to determine which shader we are going to compile.
* @param source : The shader source code.
* @return id of the shaderObject
*/
GLuint CompileShader(GLuint type, const std::string& source){
	// Compile our shaders
	GLuint shaderObject;

	// Based on the type passed in, we create a shader object specifically for that
	// type.
	if(type == GL_VERTEX_SHADER){
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}else if(type == GL_FRAGMENT_SHADER){
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}
    else if(type == GL_GEOMETRY_SHADER){
        shaderObject = glCreateShader(GL_GEOMETRY_SHADER);
    }

	const char* src = source.c_str();
	// The source of our shader
	glShaderSource(shaderObject, 1, &src, nullptr);
	// Now compile our shader
	glCompileShader(shaderObject);

	// Retrieve the result of our compilation
	int result;
	// Our goal with glGetShaderiv is to retrieve the compilation status
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE){
		int length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		char* errorMessages = new char[length]; // Could also use alloca here.
		glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

		if(type == GL_VERTEX_SHADER){
			std::cout << "ERROR: GL_VERTEX_SHADER compilation failed!\n" << errorMessages << "\n";
		}else if(type == GL_FRAGMENT_SHADER){
			std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed!\n" << errorMessages << "\n";
		}else{
            std::cout << "ERROR: Other SHADER compilation failed!\n" << errorMessages << "\n";
        }
		// Reclaim our memory
		delete[] errorMessages;

		// Delete our broken shader
		glDeleteShader(shaderObject);

		return 0;
	}

  return shaderObject;
}

/**
* Creates a graphics program object (i.e. graphics pipeline) with a Vertex Shader and a Fragment Shader
*
* @param vertexShaderSource Vertex source code as a string
* @param fragmentShaderSource Fragment shader source code as a string
* @return id of the program Object
*/
GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource){

    // Create a new program object
    GLuint programObject = glCreateProgram();

    // Compile our shaders
    GLuint myVertexShader   = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Link our two shader programs together.
	// Consider this the equivalent of taking two .cpp files, and linking them into
	// one executable file.
    glAttachShader(programObject,myVertexShader);
    glAttachShader(programObject,myFragmentShader);
    glLinkProgram(programObject);

    // Validate our program
    glValidateProgram(programObject);

    GLint success;
    glGetProgramiv(programObject, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(programObject, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }


    // Once our final program Object has been created, we can
	// detach and then delete our individual shaders.
    glDetachShader(programObject,myVertexShader);
    glDetachShader(programObject,myFragmentShader);
	// Delete the individual shaders once we are done
    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    return programObject;
}

GLuint Create3ShaderProgram(const std::string& vertexShaderSource, const std::string& geometryShaderSource, const std::string& fragmentShaderSource){

    // Create a new program object
    GLuint programObject = glCreateProgram();

    // Compile our shaders
    GLuint myVertexShader   = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint myGeometryShader = CompileShader(GL_GEOMETRY_SHADER, geometryShaderSource);

    // Link our two shader programs together.
	// Consider this the equivalent of taking two .cpp files, and linking them into
	// one executable file.
    glAttachShader(programObject,myVertexShader);
    glAttachShader(programObject,myGeometryShader);
    glAttachShader(programObject,myFragmentShader);
    glLinkProgram(programObject);

    // Validate our program
    glValidateProgram(programObject);

    // Once our final program Object has been created, we can
	// detach and then delete our individual shaders.
    glDetachShader(programObject,myVertexShader);
    glDetachShader(programObject,myGeometryShader);
    glDetachShader(programObject,myFragmentShader);
	// Delete the individual shaders once we are done
    glDeleteShader(myVertexShader);
    glDeleteShader(myGeometryShader);
    glDeleteShader(myFragmentShader);

    return programObject;
}

/**
 * Creates a array of possible (x,z) coordinates to place objects and return an array 
 * with 4 random coordinates that can be used to place 4 objects
 * 
 * @return array of 4 random coordinates
*/
std::vector<glm::vec2> RandomObjectsPlacement() {
    std::vector<glm::vec2> selectableCoordsArray; // x, z coordinate

    // Map defined as 40.f by 40.f, possible placement for objects, min 5.f apart from each other and from the boundary
    for (float x = -15.f; x <= 15.f; x += 5.0f) {
        for (float y = -15.f; y <= 15.f; y += 5.0f) {
            // exclude origin
            if (!(x >= -0.5f && x <= 0.5f && y >= -0.5f && y <= 0.5f)) {
                selectableCoordsArray.push_back(glm::vec2(x, y));
            }
        }
    }

    // Shuffle the array
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(selectableCoordsArray.begin(), selectableCoordsArray.end(), g);

    // Pick the first 4 nonidentical glm::vec2
    std::vector<glm::vec2> selectedVecs(selectableCoordsArray.begin(), selectableCoordsArray.begin() + 4);
    return selectedVecs;
}

/**
 * Creates a array of possible (x,z) coordinates to place trees and return an array 
 * with random coordinates that can be used to place trees without interfering with structure objects
 * @param objectsCoords reserved coords for objects
 * @param numOfTrees number of trees to be placed, default as 40
 * 
 * @return array of 40 random coordinates
*/
std::vector<glm::vec2> RandomTreesPlacement(std::vector<glm::vec2>& objectsCoords, int numOfTrees) {
    std::vector<glm::vec2> selectedVecs;    // x, z coordinate
    
    std::random_device rd;
    std::mt19937 gen(rd());
    // Define the range
    std::uniform_real_distribution<float> dis(-19.5, 19.5);

    for (int i = 0; i <= numOfTrees; i ++) {
        // Generate random coordinates
        float x = dis(gen);
        float y = dis(gen);
        bool isValid = true;
        // Exclude origin 
        if (x != 0.f && y != 0.f) {
            // Check against previous selected objects coords
            for (const auto& objCoord : objectsCoords) {
                float distance = glm::length(glm::vec2(x, y) - objCoord);
                if (distance < 2.0f) {
                    isValid = false;
                    break;
                }
            }

            // Only select the valid coords for trees
            if (isValid) {
                selectedVecs.push_back(glm::vec2(x, y));
            }
        }
    }

    return selectedVecs;
}
