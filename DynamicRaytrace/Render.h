#ifndef RENDER_H
#define RENDER_H

#include "Includes.h"

class Texture {
	SDL_Texture* surface;
	SDL_Texture* bump;
public:
	Texture();
	Texture(std::string textureName, bool hasBump);

	SDL_Texture* getSurface();
	SDL_Texture* getBump();
};

SDL_Colour GetColour(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

extern SDL_Window* RENDER_WINDOW;
extern SDL_Renderer* RENDER_RENDERER;

extern SDL_Texture* RENDER_TEX_SURFACE;
extern SDL_Texture* RENDER_TEX_BUMP;
extern SDL_Texture* RENDER_TEX_UI;

extern unsigned int RENDER_WIDTH, RENDER_HEIGHT, RENDER_RESOLUTION;
extern double RENDER_FOV, RENDER_DISTANCE;

extern Texture RENDER_TEXTURE_WALL;

int RenderInit();
void RenderUpdate();
#endif