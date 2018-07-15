#include "Render.h"
#include "World.h"
#include "Entity.h"

int RenderInit() {
	RENDER_WINDOW = SDL_CreateWindow("Dynamic Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);

	if (RENDER_WINDOW == nullptr) {
		return -1;
	}

	RENDER_RENDERER = SDL_CreateRenderer(RENDER_WINDOW, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(RENDER_RENDERER, SDL_BLENDMODE_BLEND);
	return 0;
}

void RenderUpdate() {
	glm::dvec2 playerPosition = LOCALPLAYER.getEntityPosition();
	double playerYaw = LOCALPLAYER.getYawAngles();

	//Render
	SDL_SetRenderDrawColor(RENDER_RENDERER, 190, 220, 240, 255);
	SDL_RenderClear(RENDER_RENDERER);

	//render raytrace
	int maxTraces = RENDER_WIDTH / RENDER_RESOLUTION;

	for (int xTrace = 0; xTrace < maxTraces; xTrace++) {
		double rayAngle = (playerYaw - RENDER_FOV / 2.0) + ((double)xTrace / (double)maxTraces) * RENDER_FOV;
		double rayDirX = sin(rayAngle);
		double rayDirY = cos(rayAngle);

		double rayDistance = 0.0;

		std::vector<Tile*> hits = std::vector<Tile*>();

		bool hitSolid = false;

		int coordX = (int)(playerPosition.x);
		int coordY = (int)(playerPosition.y);

		int lastSuccessfulCoordX = -999999, lastSuccessfulCoordY = -999999;

		while (!hitSolid &&
				rayDistance < RENDER_DISTANCE &&
				coordX >= 0 &&
				coordX < WORLD_MAPSIZE &&
				coordY >= 0 &&
				coordY < WORLD_MAPSIZE) {

			rayDistance += 0.01;

			if (WORLD_MAP[coordX][coordY] != nullptr &&
				!(lastSuccessfulCoordX == coordX &&
				lastSuccessfulCoordY == coordY )) {

				hitSolid = !WORLD_MAP[coordX][coordY]->isTransparent();
				WORLD_MAP[coordX][coordY]->distanceToPlayer = rayDistance;
				hits.push_back(WORLD_MAP[coordX][coordY]);
				
				lastSuccessfulCoordX = coordX;
				lastSuccessfulCoordY = coordY;
			}
			coordX = (int)(playerPosition.x + rayDirX * rayDistance);
			coordY = (int)(playerPosition.y + rayDirY * rayDistance);
		}

		if (!hits.empty()) {
			SDL_Rect traceRect;
			double distortionFix = cos(rayAngle - playerYaw);

			traceRect.w = RENDER_RESOLUTION;
			traceRect.x = xTrace * RENDER_RESOLUTION;

			for (int i = hits.size() - 1; i >= 0; i--) {

				Tile* tile = hits.at(i);
				rayDistance = tile->distanceToPlayer * distortionFix;

				int rectHeight = (int)((float)RENDER_HEIGHT / rayDistance), rectTransparency = (int)(tile->renderColour().a / (rayDistance / 4.0));
				if (rectHeight > RENDER_HEIGHT) {
					rectHeight = RENDER_HEIGHT;
				}
				if (rectTransparency > tile->renderColour().a) {
					rectTransparency = tile->renderColour().a;
				}

				
				traceRect.h = rectHeight;
				traceRect.y = (RENDER_HEIGHT - traceRect.h) / 2 + (int)((double)traceRect.h * LOCALPLAYER.getEntityPosition().z);

				SDL_SetRenderDrawColor(RENDER_RENDERER, tile->renderColour().r, tile->renderColour().g, tile->renderColour().b, rectTransparency);
				SDL_RenderFillRect(RENDER_RENDERER, &traceRect);
			}
		}
	}

	//render terrain map
	{
		SDL_Rect currentMapBackground;
		currentMapBackground.w = WORLD_MAPSIZE * 10;
		currentMapBackground.h = WORLD_MAPSIZE * 10;
		currentMapBackground.x = 0;
		currentMapBackground.y = 0;
		SDL_SetRenderDrawColor(RENDER_RENDERER, 255, 255, 255, 128);
		SDL_RenderFillRect(RENDER_RENDERER, &currentMapBackground);
		SDL_SetRenderDrawColor(RENDER_RENDERER, 255, 0, 0, 128);


		for (int x = 0; x < WORLD_MAPSIZE; x++) {
			for (int y = 0; y < WORLD_MAPSIZE; y++) {
				if (WORLD_MAP[x][y] != nullptr) {
					SDL_Rect currentMapRect;
					currentMapRect.w = 10;
					currentMapRect.h = 10;
					currentMapRect.x = 10 * x;
					currentMapRect.y = 10 * y;
					SDL_SetRenderDrawColor(RENDER_RENDERER, 0, 0, 0, 128);
					SDL_RenderFillRect(RENDER_RENDERER, &currentMapRect);
				}
			}
		}

		//render map player
		SDL_Rect playerMapRect;
		playerMapRect.w = 6;
		playerMapRect.h = 6;
		playerMapRect.x = (playerPosition.x * 10) - 3;
		playerMapRect.y = (playerPosition.y * 10) - 3;
		SDL_SetRenderDrawColor(RENDER_RENDERER, 255, 0, 0, 128);
		SDL_RenderFillRect(RENDER_RENDERER, &playerMapRect);

		//render map player view
		double playerLookPointX = (playerPosition.x * 10) + (sin(playerYaw) * 20);
		double playerLookPointY = (playerPosition.y * 10) + (cos(playerYaw) * 20);
		SDL_SetRenderDrawColor(RENDER_RENDERER, 0, 0, 255, 128);
		SDL_RenderDrawLine(RENDER_RENDERER, (playerPosition.x * 10), (playerPosition.y * 10), playerLookPointX, playerLookPointY);
	}

	SDL_RenderPresent(RENDER_RENDERER);
}

SDL_Colour GetColour(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_Colour foo;
	foo.r = r;
	foo.g = g;
	foo.b = b;
	foo.a = a;
	return foo;
}
