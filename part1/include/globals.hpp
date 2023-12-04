#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL.h>
#endif

#include "OBJ.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Texture.hpp"

// Forward Declaration
struct STLFile;

struct Global{
	// Screen Dimensions
	int gScreenWidth 						= 1200;
	int gScreenHeight 						= 720;
	SDL_Window* gGraphicsApplicationWindow 	= nullptr;
	SDL_GLContext gOpenGLContext			= nullptr;

	// xz plane dimension boundary of map
	float gMaxValue 						= 25.f;
	float gMinValue							= -25.f;

	// Main loop flag
	bool gQuit = false; // If this is quit = 'true' then the program terminates.

	// Win game flag
	bool gWin = false; 

	// Camera
	Camera gCamera;

	// Draw wireframe mode
	GLenum gPolygonMode = GL_FILL;

	// Light object
	Light gLight;

    std::vector<Light> glights;

	// Tree texture
	std::string gTreeFileName = "./../common/textures/tree.ppm";
	// House Object
	std::string gHouseFileName = "./../common/objects/house/house_obj.obj";
	// Chapel Object
	std::string gChapelFileName = "./../common/objects/chapel/chapel_obj.obj";
	// Windmill Object
	std::string gWindmillFileName = "./../common/objects/windmill/windmill.obj";
	// Chalice Object
	std::string gChaliceFileName = "./../common/objects/Chalice/Stone_Chalic_OBJ.obj";
	// Grass Object
	std::string gGrassFileName = "./../common/objects/grass/grass.obj";
	// Tree Objects
	std::string gTreeFileName1 = "./../common/objects";
	std::string gTreeFileName2 = "./../common/objects";
	std::string gTreeFileName3 = "./../common/objects";
};

// External linkage such that the
// global variable is available
// everywhere.
extern Global g;

#endif
