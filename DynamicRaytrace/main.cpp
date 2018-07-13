#include "Includes.h"
#include "MathHelper.h"
#include "World.h"

SDL_Window* mainWindow;
SDL_Renderer* mainRenderer;

const unsigned int ChunkMaxSize = 16, ChunkMaxTotalSize = ChunkMaxSize * ChunkMaxSize;

unsigned int screenWidth = 1280, screenHeight = 720, renderResolution = 1;
double renderFOV = 70.0 * (M_PI / 180.0), renderDistance = 100.0;

typedef unsigned int TileID;
TileID chunk[ChunkMaxSize][ChunkMaxSize] = { 0 };

glm::dvec2 playerPosition, playerAcceleration;
double playerYaw = M_PI / 2;

int main(int argc, char** argv) {
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		return -1;
	}

	mainWindow = SDL_CreateWindow("Dynamic Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);

	if (mainWindow == nullptr) {
		return -1;
	}

	mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	//generate map
	for (int x = 0; x < ChunkMaxSize; x++) {
		for (int y = 0; y < ChunkMaxSize; y++) {
			if ((x == 0 || x == ChunkMaxSize - 1) || (y == 0 || y == ChunkMaxSize - 1)) {
				chunk[x][y] = 1;
			}
		}
	}
	chunk[2][2] = 2;
	chunk[2][3] = 2;
	chunk[2][4] = 2;
	chunk[3][4] = 2;
	chunk[2][15] = 0;

	//set player position
	playerPosition.x = 8.0;
	playerPosition.y = 8.0;

	//update loop
	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaTime = 0;

	bool input_forward = false,
		input_backward = false,
		input_left = false,
		input_right = false;

	bool lockMouse = true;

	bool update = true, firstIteration = true;
	while (update) {

		//Input
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				update = false;
			}

			if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
				if (e.key.keysym.scancode == SDL_SCANCODE_W) {
					input_forward = e.type == SDL_KEYDOWN;
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_A) {
					input_left = e.type == SDL_KEYDOWN;
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_S) {
					input_backward = e.type == SDL_KEYDOWN;
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_D) {
					input_right = e.type == SDL_KEYDOWN;
				}
			}
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					lockMouse = !lockMouse;
					SDL_SetRelativeMouseMode(lockMouse ? SDL_TRUE : SDL_FALSE);

					if(!lockMouse)
						SDL_WarpMouseInWindow(mainWindow, screenWidth / 2, screenHeight / 2);
				}
			}

			if (e.type == SDL_MOUSEMOTION) {
				if (lockMouse) {
					playerYaw += ((double)e.motion.xrel / 100.0);
					WrapAngles(&playerYaw);
				}
			}
		}

		//Logic
		//handle player movement
		playerPosition.x += playerAcceleration.x;
		playerPosition.y += playerAcceleration.y;

		playerAcceleration.x = 0;
		playerAcceleration.y = 0;

		if (input_forward) {
			playerAcceleration.x += sin(playerYaw);
			playerAcceleration.y += cos(playerYaw);
		}
		if (input_backward) {
			playerAcceleration.x += -sin(playerYaw);
			playerAcceleration.y += -cos(playerYaw);
		}
		if (input_left) {
			playerAcceleration.x += sin(playerYaw - M_PI / 2);
			playerAcceleration.y += cos(playerYaw - M_PI / 2);
		}
		if (input_right) {
			playerAcceleration.x += sin(playerYaw + M_PI / 2);
			playerAcceleration.y += cos(playerYaw + M_PI / 2);
		}

		playerAcceleration.x *= deltaTime * 8 * 10;
		playerAcceleration.y *= deltaTime * 8 * 10;

		//basic collision detection
		for (int i = 0; i < ChunkMaxTotalSize; i++) {
			//if player will accelerate within bounds of chunk
			if ((int)(playerPosition.x + playerAcceleration.x) >= 0 && (int)(playerPosition.x + playerAcceleration.x) <= ChunkMaxSize &&
				(int)(playerPosition.y + playerAcceleration.y) >= 0 && (int)(playerPosition.y + playerAcceleration.y) <= ChunkMaxSize) {
				//if player will accelerate into solid tile
				if (chunk[(int)(playerPosition.x + playerAcceleration.x)][(int)(playerPosition.y + playerAcceleration.y)] > 0) {
					playerAcceleration.x = 0.0;
					playerAcceleration.y = 0.0;
				}
			}
			else { //otherwise stop the player leaving the chunk for now
				///Here, we should have the logic for moving the player into a new chunk, but that is a whole other beast to tackle.
				playerAcceleration.x = 0.0;
				playerAcceleration.y = 0.0;
			}
		}


		//Render
		SDL_SetRenderDrawColor(mainRenderer, 190, 220, 240, 255);
		SDL_RenderClear(mainRenderer);

		//render raytrace
		int maxTraces = screenWidth / renderResolution;

		for (int xTrace = 0; xTrace < maxTraces; xTrace++) {
			double rayAngle = (playerYaw - renderFOV / 2.0) + ((double)xTrace / (double)maxTraces) * renderFOV;
			double rayDirX = sin(rayAngle);
			double rayDirY = cos(rayAngle);

			double rayDistance = 0.0;

			bool hitWall = false;

			while (!hitWall && rayDistance < renderDistance) {
				rayDistance += 0.01;

				int coordX = (int)(playerPosition.x + rayDirX * rayDistance);
				int coordY = (int)(playerPosition.y + rayDirY * rayDistance);

				//Test if ray is within chunk bounds
				if (coordX >= 0 && coordX < ChunkMaxSize && coordY >= 0 && coordY < ChunkMaxSize) {
					if (chunk[coordX][coordY] > 0) {
						hitWall = true;

						//Find sqrt of distance..
						/*double distVecX = rayDirX * rayDistance;
						double distVecY = rayDirY * rayDistance;
						rayDistance = sqrt(distVecX * distVecX + distVecY * distVecY);*/
					}
				}
			}

			if (hitWall) {

				//correct distortion
				rayDistance = rayDistance * cos(rayAngle - playerYaw);

				int rectHeight = (int)((float)screenHeight / rayDistance), rectTransparency = (int)(255.0 / (rayDistance / 4.0));
				if (rectHeight > screenHeight) {
					rectHeight = screenHeight;
				}
				if (rectTransparency > 255) {
					rectTransparency = 255;
				}

				SDL_Rect traceRect;
				traceRect.w = renderResolution;
				traceRect.h = rectHeight;
				traceRect.x = xTrace * renderResolution;
				traceRect.y = (screenHeight - traceRect.h) / 2;

				SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, rectTransparency);
				SDL_RenderFillRect(mainRenderer, &traceRect);
			}
		}

		//render terrain map
		{
			SDL_Rect currentMapBackground;
			currentMapBackground.w = ChunkMaxSize * 10;
			currentMapBackground.h = ChunkMaxSize * 10;
			currentMapBackground.x = 0;
			currentMapBackground.y = 0;
			SDL_SetRenderDrawColor(mainRenderer, 255, 255, 255, 128);
			SDL_RenderFillRect(mainRenderer, &currentMapBackground);
			SDL_SetRenderDrawColor(mainRenderer, 255, 0, 0, 128);
			

			for (int x = 0; x < ChunkMaxSize; x++) {
				for (int y = 0; y < ChunkMaxSize; y++) {
					if (chunk[x][y] > 0) {
						SDL_Rect currentMapRect;
						currentMapRect.w = 10;
						currentMapRect.h = 10;
						currentMapRect.x = 10 * x;
						currentMapRect.y = 10 * y;
						SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 128);
						SDL_RenderFillRect(mainRenderer, &currentMapRect);
					}
				}
			}

			//render map player
			SDL_Rect playerMapRect;
			playerMapRect.w = 6;
			playerMapRect.h = 6;
			playerMapRect.x = (playerPosition.x * 10) - 3;
			playerMapRect.y = (playerPosition.y * 10) - 3;
			SDL_SetRenderDrawColor(mainRenderer, 255, 0, 0, 128);
			SDL_RenderFillRect(mainRenderer, &playerMapRect);

			//render map player view
			double playerLookPointX = (playerPosition.x * 10) + (sin(playerYaw) * 20);
			double playerLookPointY = (playerPosition.y * 10) + (cos(playerYaw) * 20);
			SDL_SetRenderDrawColor(mainRenderer, 0, 0, 255, 128);
			SDL_RenderDrawLine(mainRenderer, (playerPosition.x * 10), (playerPosition.y * 10), playerLookPointX, playerLookPointY);
		}

		SDL_RenderPresent(mainRenderer);
		firstIteration = false;

		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceCounter());
	}

	SDL_Quit();
	return 0;
}