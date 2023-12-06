/* Compilation on Linux: 
 g++ -std=c++17 ./src/*.cpp -o prog -I ./include/ -I./../common/thirdparty/ -lSDL2 -ldl
*/

// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

// C++ Standard Template Library (STL)
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// Our libraries
#include "Camera.hpp"
#include "OBJ.hpp"
#include "Light.hpp"
#include "util.hpp"
#include "BillboardList.hpp"
// vvvvvvvvvvvvvvvvvvvvvvvvvv Globals vvvvvvvvvvvvvvvvvvvvvvvvvv
// Globals generally are prefixed with 'g' in this application.
#include "globals.hpp"

std::vector<OBJ*> gObjVector;
std::vector<OBJ*> gBatteryOBJs;
OBJ* grass;
std::vector<glm::vec2> gSelectedVecs;
std::vector<glm::vec2> gTreesCoords;
std::vector<BillboardList*> gTrees;

/**
* Initialization of the graphics application. Typically this will involve setting up a window
* and the OpenGL Context (with the appropriate version)
*
* @return void
*/
void InitializeProgram(){
	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}
	
	// Setup the OpenGL Context
	// Use OpenGL 4.1 core or greater
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	// We want to request a double buffer for smooth updating.
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );

	// Create an application window using OpenGL that supports SDL
	g.gGraphicsApplicationWindow = SDL_CreateWindow( "Deep In Woods",
													SDL_WINDOWPOS_UNDEFINED,
													SDL_WINDOWPOS_UNDEFINED,
													g.gScreenWidth,
													g.gScreenHeight,
													SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

	// Check if Window did not create.
	if( g.gGraphicsApplicationWindow == nullptr ){
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}

	// Create an OpenGL Graphics Context
	g.gOpenGLContext = SDL_GL_CreateContext( g.gGraphicsApplicationWindow );
	if( g.gOpenGLContext == nullptr){
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}

	// Initialize GLAD Library
	if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
		std::cout << "glad did not initialize" << std::endl;
		exit(1);
	}

    // // Initialize Light
    // g.gLight.Initialize();

    for(auto& Battery : g.gBatteries){
        Battery.Initialize();
    }

    for(int i = 0; i < 10; ++i){
        gBatteryOBJs.push_back(new OBJ(g.gBatteryFileName));
    }
	for (auto& object : gBatteryOBJs) {
		object->Initialize();
        object->randomXZCoord(-20, 20);
	}
    // std::cout << gBatteryOBJs.size() << " Batteries are available to pick up. Good Luck!" << std::endl;

	// Initialize objects
	gObjVector.push_back(new OBJ(g.gHouseFileName));
	gObjVector.push_back(new OBJ(g.gChapelFileName));
	gObjVector.push_back(new OBJ(g.gWindmillFileName));
	gObjVector.push_back(new OBJ(g.gChaliceFileName));
	
	for (auto& object : gObjVector) {
		object->Initialize();
	}

	std::cout << "Generating forest, please wait..." << std::endl;

	// Initialize coordinates to place objects
	gSelectedVecs = RandomObjectsPlacement();

	// Initialize 4 kinds of Trees 
	gTrees.push_back(new BillboardList(g.gTreeFileName));
	gTrees.push_back(new BillboardList(g.gTreeFileName1));
	gTrees.push_back(new BillboardList(g.gTreeFileName2));
	gTrees.push_back(new BillboardList(g.gTreeFileName3));

	for (auto& tree : gTrees) {
		std::vector<glm::vec2> treeCoords = RandomTreesPlacement(gSelectedVecs, 100);
    	gTreesCoords.insert(gTreesCoords.end(), treeCoords.begin(), treeCoords.end());
		tree->SetPos(treeCoords);
    	tree->Initialize();
	}
   
	// Initialize Grass
	grass = new OBJ(g.gGrassFileName);
	grass->Initialize();

	std::cout << gBatteryOBJs.size() << " Batteries are available to pick up. Good Luck!" << std::endl;
}

/**
* PreDraw
* Typically we will use this for setting some sort of 'state'
* Note: some of the calls may take place at different stages (post-processing) of the
* 		 pipeline.
* @return void
*/
void PreDraw(){
	// Disable depth test and face culling.
    glEnable(GL_DEPTH_TEST);                    // NOTE: Need to enable DEPTH Test
    glDisable(GL_CULL_FACE);

    // Set the polygon fill mode     
    glPolygonMode(GL_FRONT_AND_BACK,g.gPolygonMode);

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, g.gScreenWidth, g.gScreenHeight);
    glClearColor( 0.0f, 0.0f, 0.0f, 1.f );

    // Clear color buffer and Depth Buffer
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}


/**
* Draw
* The render function gets called once per loop.
* Typically this includes 'glDraw' related calls, and the relevant setup of buffers
* for those calls.
*
* @return void
*/
void Draw(){
    // Draw objects
    g.gCamera.CheckBattery();

    //std::cout << "(OBJ) start Draw" << std::endl;
	// House
	gObjVector[0]->PreDraw(glm::vec3(gSelectedVecs[0].x, -gObjVector[0]->getMinCoord().y, gSelectedVecs[0].y), 30.f);
	gObjVector[0]->Draw();

	// Chapel
	gObjVector[1]->PreDraw(glm::vec3(gSelectedVecs[1].x, -gObjVector[1]->getMinCoord().y, gSelectedVecs[1].y), 90.f);
    gObjVector[1]->Draw();

	// Windmill
	gObjVector[2]->PreDraw(glm::vec3(gSelectedVecs[2].x, -gObjVector[2]->getMinCoord().y, gSelectedVecs[2].y), 45.f);
    gObjVector[2]->Draw();

	// Chalice
	gObjVector[3]->PreDraw(glm::vec3(gSelectedVecs[3].x, -gObjVector[3]->getMinCoord().y, gSelectedVecs[3].y));
    gObjVector[3]->Draw();

	// Grass
	grass->PreDraw(glm::vec3(0.0f, 0.0f, 0.0f));
	grass->Draw();

    // Draw trees
	for (auto& tree : gTrees) {
		tree->PreDraw();
    	tree->Draw();
	}

    for(int i = 0; i < gBatteryOBJs.size(); ++i){
        if(gBatteryOBJs[i]->getObjectCoord().y != 0) gBatteryOBJs[i]->PreDraw(gBatteryOBJs[i]->getObjectCoord());
        else gBatteryOBJs[i]->PreDraw(glm::vec3(gBatteryOBJs[i]->getObjectCoord().x, -gBatteryOBJs[i]->getMinCoord().y, gBatteryOBJs[i]->getObjectCoord().z));
        gBatteryOBJs[i]->Draw();
    }

    // Draw light
    //g.gLight.PreDraw();
    //g.gLight.Draw();
}

/**
* Helper Function to get OpenGL Version Information
*
* @return void
*/
void getOpenGLVersionInfo(){
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}

/**
 * Function to check if camera is in obj, 
 * used to detect collision, collect battery and reach final goals (Chalic)
 * 
 * @return bool whether camera in obj
*/
bool InOBJ(glm::vec3 cameraEyePosition, OBJ* object, float margin=0.1f){
	// 1. Translate the cameraEyePosition so that the center of rotation is at the origin.
	// 2. Rotate the translated cameraEyePosition using a rotation matrix.
	// 3. Compare with the object's original max and min coord
    glm::mat4 translationMatrix1 = glm::translate(glm::mat4(1.0f), -object->getObjectCoord());
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -glm::radians(object->getRot()), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 updatePointMatrix = rotationMatrix * translationMatrix1;
    glm::vec3 rotatedCameraEyePosition = glm::vec3(updatePointMatrix * glm::vec4(cameraEyePosition, 1.0f));
    return rotatedCameraEyePosition.x <= object->getMaxCoord().x + margin
		&& rotatedCameraEyePosition.z <= object->getMaxCoord().z + margin
		&& rotatedCameraEyePosition.x >= object->getMinCoord().x - margin
		&& rotatedCameraEyePosition.z >= object->getMinCoord().z - margin;
}

/**
 * Function to check collision with tress
 * 
 * @return bool whether camera collide in tree
*/
bool CollideTree(glm::vec3 cameraEyePosition, glm::vec2 treeCoord, float margin=0.2f){
    return cameraEyePosition.x <= treeCoord.x + margin
		&& cameraEyePosition.z <= treeCoord.y + margin
		&& cameraEyePosition.x >= treeCoord.x - margin
		&& cameraEyePosition.z >= treeCoord.y - margin;
}

/**
 * Function to check if found Chalice (reached goal), it has a larger margin than InOBJ check
 * 
 * @return bool whether we have reached goal
*/
bool FoundChalice(glm::vec3 cameraEyePosition, OBJ* chalice) {
	return InOBJ(cameraEyePosition, chalice, 0.2f);
}

/**
 * Function to check collision between objects and camera
 * 
 * @return bool whether we have a collision
*/
bool HasCollision(glm::vec3 cameraEyePosition, std::vector<OBJ*> gObjVector) {
	// collision with object
	for (auto& object : gObjVector) {
		if (InOBJ(cameraEyePosition, object)) {
			return true;
		}
	}

	// collision with tree
	for (auto& tree : gTreesCoords) {
		if (CollideTree(cameraEyePosition, tree)) {
			return true;
		}
	}

	// collision with boundary
	return !(cameraEyePosition.x >= g.gMinValue 
			&& cameraEyePosition.x <= g.gMaxValue 
			&& cameraEyePosition.z >= g.gMinValue 
			&& cameraEyePosition.z <= g.gMaxValue);
}

/**
 * Function to handle event right after winning the game
*/
void WinGame() {
	// TODO: in the future we can add particle effects such as fire or fireworks here
	std::cout << "You Win!" << std::endl;
}


/**
* Function called in the Main application loop to handle user input
*
* @return void
*/
void Input(){ 
    // Two static variables to hold the mouse position
    static int mouseX=g.gScreenWidth/2;
    static int mouseY=g.gScreenHeight/2; 

	// Event handler that handles various events in SDL
	// that are related to input and output
	SDL_Event e;
	//Handle events on queue
	while(SDL_PollEvent( &e ) != 0){
		// If users posts an event to quit
		if(e.type == SDL_QUIT){
			std::cout << "Goodbye! (Leaving MainApplicationLoop())" << std::endl;
			g.gQuit = true;
		}
        if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
			std::cout << "ESC: Goodbye! (Leaving MainApplicationLoop())" << std::endl;
            g.gQuit = true;
        }
		if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q){
			std::cout << "Q: Goodbye! (Leaving MainApplicationLoop())" << std::endl;
            g.gQuit = true;
        }
        if(e.type==SDL_MOUSEMOTION){
            // Capture the change in the mouse position
            mouseX+=e.motion.xrel;
            mouseY+=e.motion.yrel;
            g.gCamera.MouseLook(mouseX,mouseY);
        }
	}

    // Retrieve keyboard state
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Camera
    // Update our position of the camera, move character with WASD
    float cameraSpeed = 0.02f;
    if (state[SDL_SCANCODE_W]) {
		if (HasCollision(g.gCamera.CheckForward(cameraSpeed), gObjVector)) {
			// move on the opposite direction when in collision and was moving forward
			g.gCamera.MoveBackward(cameraSpeed);
		} else {
			g.gCamera.MoveForward(cameraSpeed);
			if (FoundChalice(g.gCamera.GetEyePosition(), gObjVector[3])) {
				g.gWin = true;
			}
		}
    }
    if (state[SDL_SCANCODE_S]) {
        if (HasCollision(g.gCamera.CheckBackward(cameraSpeed), gObjVector)) {
			g.gCamera.MoveForward(cameraSpeed);
		} else {
			g.gCamera.MoveBackward(cameraSpeed);
			if (FoundChalice(g.gCamera.GetEyePosition(), gObjVector[3])) {
				g.gWin = true;
			}
		}
    }
    if (state[SDL_SCANCODE_A]) {
		if (HasCollision(g.gCamera.CheckLeft(cameraSpeed), gObjVector)) {
			// g.gCamera.MoveRight(cameraSpeed);
		} else {
			g.gCamera.MoveLeft(cameraSpeed);
			if (FoundChalice(g.gCamera.GetEyePosition(), gObjVector[3])) {
				g.gWin = true;
			}
		}
    }
    if (state[SDL_SCANCODE_D]) {
		if (HasCollision(g.gCamera.CheckRight(cameraSpeed), gObjVector)) {
			// g.gCamera.MoveLeft(cameraSpeed);
		} else {
			g.gCamera.MoveRight(cameraSpeed);
			if (FoundChalice(g.gCamera.GetEyePosition(), gObjVector[3])) {
				g.gWin = true;
			}
		}
    } 

	// Press R to reset position
	if (state[SDL_SCANCODE_R]) {
		SDL_Delay(250); 
		g.gCamera.SetCameraEyePosition(g.gCamera.GetEyeInitialPosition().x,
		 								g.gCamera.GetEyeInitialPosition().y,
		  								g.gCamera.GetEyeInitialPosition().z);
	}

	// Press G to teleport player near Chalice
	if (state[SDL_SCANCODE_G]) {
		SDL_Delay(250); 
		g.gCamera.SetCameraEyePosition(gSelectedVecs[3].x+1.f,
		 								g.gCamera.GetEyeInitialPosition().y,
		  								gSelectedVecs[3].y+1.f);
		// g.gCamera.SetViewDirection(-1.0f, -0.5f, -1.0f);
	}

	// Press TAB to switch between fill and line mode
    if (state[SDL_SCANCODE_TAB]) {
        SDL_Delay(250); // This is hacky in the name of simplicity,
                       // but we just delay the
                       // system by a few milli-seconds to process the 
                       // keyboard input once at a time.
        if(g.gPolygonMode == GL_FILL){
            g.gPolygonMode = GL_LINE;
        }else{
            g.gPolygonMode = GL_FILL;
        }
    }
    
    // check whether get the battery
    glm::vec3 curPos = g.gCamera.GetEyePosition();
    for(int i = 0; i < gBatteryOBJs.size(); ++i){
        if(InOBJ(curPos, gBatteryOBJs[i])){
            g.gCamera.CollectBattery();
            std::cout << "Collected Battery!" << std::endl;
            delete gBatteryOBJs[i];
            gBatteryOBJs.erase(gBatteryOBJs.begin() + i);
            g.gCamera.GetBatteryInfo();
            break;
        }
    }
    // camera out of battery for 10 sec, Game Over
    if(g.gCamera.GetGameOver())
        g.gQuit = true;
}


/**
* Main Application Loop
* This is an infinite loop in our graphics application
*
* @return void
*/
void MainLoop(){

    // Little trick to map mouse to center of screen always.
    // Useful for handling 'mouselook'
    // This works because we effectively 're-center' our mouse at the start
    // of every frame prior to detecting any mouse motion.
    SDL_WarpMouseInWindow(g.gGraphicsApplicationWindow, g.gScreenWidth/2, g.gScreenHeight/2);
    SDL_SetRelativeMouseMode(SDL_TRUE);


	// While application is running
	while(!g.gQuit){
        // Type of start of frame
		Uint32 start = SDL_GetTicks();
		
		// Setup anything (i.e. OpenGL State) that needs to take
		// place before draw calls

		if (g.gWin) {
			WinGame();
			SDL_Delay(1000); 
			g.gQuit = true;
		}

		PreDraw();
		// Draw Calls in OpenGL
        // When we 'draw' in OpenGL, this activates the graphics pipeline.
        // i.e. when we use glDrawElements or glDrawArrays,
        //      The pipeline that is utilized is whatever 'glUseProgram' is
        //      currently binded.
		Draw();

		// Handle Input
        Input();
        // Calculate how much time has elapsed
		// since the start of the frame, and delay
		// for the difference in milliseconds to attempt
		// to get 60 fps for systems that run too fast
		Uint32 elapsedTime = SDL_GetTicks() - start;
		if(elapsedTime < 16){
			SDL_Delay(16-elapsedTime);
		}

		//Update screen of our specified window
		SDL_GL_SwapWindow(g.gGraphicsApplicationWindow);
	}
}



/**
* The last function called in the program
* This functions responsibility is to destroy any global
* objects in which we have create dmemory.
*
* @return void
*/
void CleanUp(){
	//Destroy our SDL2 Window
	SDL_DestroyWindow(g.gGraphicsApplicationWindow);
	g.gGraphicsApplicationWindow = nullptr;

	//Delete all objects 
	for (auto& object : gObjVector) {
        delete object;
    }
	gObjVector.clear();

	for (auto& tree : gTrees) {
        delete tree;
    }
	gTrees.clear();

	for (auto& battery : gBatteryOBJs) {
		delete battery;
	}
	gBatteryOBJs.clear();
	gSelectedVecs.clear();
	gTreesCoords.clear();
	
	delete grass;

	//Quit SDL subsystems
	SDL_Quit();
}


/**
* The entry point into our C++ programs.
*
* @return program status
*/
int main( int argc, char* args[] ){
	std::cout << "Your goal is to find the Chalice!\n";
    std::cout << "Use WASD keys to move forward, left, backward, and right\n";
	std::cout << "Use mouse to look around\n";
    std::cout << "Use TAB to toggle wireframe\n";
	std::cout << "Press R to reset player position\n";
	std::cout << "Press G to teleport player near Chalice (Cheat)\n";
    std::cout << "Press ESC to quit\n";

	// 1. Setup the graphics program
	std::cout << "Generating environment, please wait..." << std::endl;
	InitializeProgram();

	// 2. Call the main application loop
	MainLoop();	

	// 3. Call the cleanup function when our program terminates
	CleanUp();

	return 0;
}
