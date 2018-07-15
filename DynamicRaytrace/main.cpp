#include "Includes.h"
#include "MathHelper.h"
#include "Input.h"
#include "World.h"
#include "Entity.h"
#include "Render.h"

//Initialise external variables.
//const unsigned int WORLD_MAPSIZE = 16, WORLD_MAPARRAYSIZE = WORLD_MAPSIZE * WORLD_MAPSIZE;
Tile* WORLD_MAP[WORLD_MAPSIZE][WORLD_MAPSIZE] = { nullptr };

SDL_Window* RENDER_WINDOW = nullptr;
SDL_Renderer* RENDER_RENDERER = nullptr;
unsigned int RENDER_WIDTH = 1280, RENDER_HEIGHT = 720, RENDER_RESOLUTION = 4;
double RENDER_FOV = 70.0 * (M_PI / 180.0), RENDER_DISTANCE = 22.7;

EntityPlayer LOCALPLAYER = EntityPlayer(0, glm::dvec3(8.0, 8.0, 0.0), M_PI / 2.0, std::string("entityliving.localplayer"), 100, 100, std::string("localplayer"));

bool	INPUT_LOCKMOUSE = true, 
		INPUT_JUMP = false,
		INPUT_FORWARD = false,
		INPUT_BACKWARD = false,
		INPUT_LEFT = false,
		INPUT_RIGHT = false;

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		return -1;
	}

	int renderInitSuccess = RenderInit();
	if (renderInitSuccess < 0) {
		return renderInitSuccess;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);

	//generate map
	int worldInitSuccess = WorldInit();
	if (worldInitSuccess < 0) {
		return worldInitSuccess;
	}

	//update loop
	Uint32 previousTicks = 0, currentTicks = 0;
	double deltaTime;

	bool update = true;
	while (update) {
		previousTicks = currentTicks;
		currentTicks = SDL_GetTicks();
		deltaTime = (double)(currentTicks - previousTicks) / 1000.0;

		update = InputUpdate() && WorldUpdate();

		//Logic
		LOCALPLAYER.update(deltaTime);

		//Render
		RenderUpdate();
	}

	SDL_Quit();
	return 0;
}