#include "Render.h"
#include "World.h"
#include "Entity.h"

Texture RENDER_TEXTURE_WALL;

int RenderInit() {

	//Initialise SDL_image. (Error otherwise.)
	int imgInitFlags = IMG_INIT_PNG;
	if (IMG_Init(imgInitFlags) != imgInitFlags) {
		return -1;
	}

	//Initialise the main window. (Error otherwise.)
	RENDER_WINDOW = SDL_CreateWindow("Dynamic Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RENDER_WIDTH, RENDER_HEIGHT, SDL_WINDOW_SHOWN);
	if (RENDER_WINDOW == nullptr) {
		return -1;
	}

	//Initialise the main renderer.(Error otherwise.)
	RENDER_RENDERER = SDL_CreateRenderer(RENDER_WINDOW, -1, SDL_RENDERER_ACCELERATED);
	if (RENDER_RENDERER == nullptr) {
		return -1;
	}

	RENDER_TEX_SURFACE = SDL_CreateTexture(RENDER_RENDERER, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);
	RENDER_TEX_BUMP = SDL_CreateTexture(RENDER_RENDERER, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);
	RENDER_TEX_UI = SDL_CreateTexture(RENDER_RENDERER, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);

	//Set renderer variables.
	SDL_SetRenderDrawBlendMode(RENDER_RENDERER, SDL_BLENDMODE_BLEND);

	//Initialise textures.
	RENDER_TEXTURE_WALL = Texture(std::string("wall"), true);
	return 0;
}

void RenderUpdate() {
	glm::dvec2 playerPosition = LOCALPLAYER.getEntityPosition();
	double playerYaw = LOCALPLAYER.getYawAngles();

	//Render
	SDL_SetRenderTarget(RENDER_RENDERER, RENDER_TEX_SURFACE);
		SDL_SetRenderDrawColor(RENDER_RENDERER, 190, 220, 240, 255);
		SDL_RenderClear(RENDER_RENDERER);
	SDL_SetRenderTarget(RENDER_RENDERER, RENDER_TEX_BUMP);
		SDL_SetRenderDrawColor(RENDER_RENDERER, 0, 0, 0, 255);
		SDL_RenderClear(RENDER_RENDERER);
	SDL_SetRenderTarget(RENDER_RENDERER, RENDER_TEX_UI);
		SDL_SetRenderDrawColor(RENDER_RENDERER, 0, 0, 0, 0);
		SDL_SetTextureBlendMode(RENDER_TEX_UI, SDL_BLENDMODE_BLEND);
		SDL_RenderClear(RENDER_RENDERER);

	//render raytrace
	int maxXTraces = RENDER_WIDTH / RENDER_RESOLUTION;
	int maxYTraces = RENDER_HEIGHT / RENDER_RESOLUTION;

	/*for (int yTrace = 0; yTrace < maxYTraces; yTrace++) {

	}*/

	for (int xTrace = 0; xTrace < maxXTraces; xTrace++) {
		double rayAngle = (playerYaw - RENDER_FOV / 2.0) + ((double)xTrace / (double)maxXTraces) * RENDER_FOV;
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
					lastSuccessfulCoordY == coordY)) {


				hitSolid = !WORLD_MAP[coordX][coordY]->isTransparent();
				WORLD_MAP[coordX][coordY]->distanceToPlayer = rayDistance;
				hits.push_back(WORLD_MAP[coordX][coordY]);
				hits.back()->coordX = coordX;
				hits.back()->coordY = coordY;

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

				int rectHeight = (int)((float)RENDER_HEIGHT / rayDistance), rectTransparency = (int)(tile->renderColour().a / ((rayDistance) * 2)) * 4; //Out of 255 units.
				if (rectHeight > RENDER_HEIGHT) {
					rectHeight = RENDER_HEIGHT;
				}
				if (rectTransparency > tile->renderColour().a) {
					rectTransparency = tile->renderColour().a;
				}
				traceRect.h = rectHeight;
				traceRect.y = (RENDER_HEIGHT - rectHeight) / 2;


				traceRect.y += (int)((double)traceRect.h * LOCALPLAYER.getEntityPosition().z); //Render Jump/Player Height

				//Get Texture X Sample
				double centreOfTileX = (double)(tile->coordX) + 0.5;
				double centreOfTileY = (double)(tile->coordY) + 0.5;
				double accurateCoordX = (playerPosition.x + rayDirX * tile->distanceToPlayer);
				double accurateCoordY = (playerPosition.y + rayDirY * tile->distanceToPlayer);

				double hitToCentreAngle = atan2((accurateCoordY - centreOfTileY), (accurateCoordX - centreOfTileX));

				double sampleX = 0.0;
				if ((hitToCentreAngle >= -M_PI * 0.25 && hitToCentreAngle < M_PI * 0.25) ||
					(hitToCentreAngle < -M_PI * 0.75 || hitToCentreAngle >= M_PI * 0.75)){
					//North/South Checks
					sampleX = accurateCoordY - (double)tile->coordY;
				}
				if ((hitToCentreAngle >= M_PI * 0.25 && hitToCentreAngle < M_PI * 0.75) ||
					(hitToCentreAngle < -M_PI * 0.25 && hitToCentreAngle >= -M_PI * 0.75)) {
					//West/East Checks
					sampleX = accurateCoordX - (double)tile->coordX;
				}

				SDL_Rect texRect;
				texRect.w = 1;
				texRect.h = 128;
				texRect.x = (int)(128.0 * sampleX);
				texRect.y = (128 - texRect.h) / 2;


				//SDL_SetRenderDrawColor(RENDER_RENDERER, tile->renderColour().r, tile->renderColour().g, tile->renderColour().b, rectTransparency);
				//SDL_RenderFillRect(RENDER_RENDERER, &traceRect);

				//SDL_SetRenderDrawColor(RENDER_RENDERER, 255, 255, 255, 255);
				SDL_SetRenderTarget(RENDER_RENDERER, RENDER_TEX_SURFACE);
				SDL_SetTextureAlphaMod(RENDER_TEXTURE_WALL.getSurface(), rectTransparency);
				SDL_RenderCopy(RENDER_RENDERER, RENDER_TEXTURE_WALL.getSurface(), &texRect, &traceRect);

				SDL_SetRenderTarget(RENDER_RENDERER, RENDER_TEX_BUMP);
				SDL_SetTextureAlphaMod(RENDER_TEXTURE_WALL.getBump(), rectTransparency);
				SDL_RenderCopy(RENDER_RENDERER, RENDER_TEXTURE_WALL.getBump(), &texRect, &traceRect);

			}
		}
	}

	//render terrain map
	SDL_SetRenderTarget(RENDER_RENDERER, RENDER_TEX_UI);
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

	SDL_SetRenderTarget(RENDER_RENDERER, NULL);
	SDL_RenderCopy(RENDER_RENDERER, RENDER_TEX_SURFACE, NULL, NULL);
	SDL_RenderCopy(RENDER_RENDERER, RENDER_TEX_UI, NULL, NULL);
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

Texture::Texture() : bump(nullptr), surface(nullptr) {}
Texture::Texture(std::string textureName, bool hasBump) :
	Texture::Texture() {
	std::string surfaceLocation = "resources/textures/surface/";
	surfaceLocation.append(textureName);
	surfaceLocation.append(".png");
	this->surface = IMG_LoadTexture(RENDER_RENDERER, surfaceLocation.c_str());
	SDL_SetTextureBlendMode(this->surface, SDL_BLENDMODE_BLEND);

	if (hasBump) {
		std::string bumpLocation = "resources/textures/bump/";
		bumpLocation.append(textureName);
		bumpLocation.append(".png");
		this->bump = IMG_LoadTexture(RENDER_RENDERER, bumpLocation.c_str());
		SDL_SetTextureBlendMode(this->bump, SDL_BLENDMODE_BLEND);
	}
}

SDL_Texture *Texture::getSurface() {
	return surface;
}

SDL_Texture *Texture::getBump() {
	return bump;
}
