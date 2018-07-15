#ifndef RENDER_H
#define RENDER_H

#include "Includes.h"

extern SDL_Window* RENDER_WINDOW;
extern SDL_Renderer* RENDER_RENDERER;

extern unsigned int RENDER_WIDTH, RENDER_HEIGHT, RENDER_RESOLUTION;
extern double RENDER_FOV, RENDER_DISTANCE;

int RenderInit();
void RenderUpdate();

SDL_Colour GetColour(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

#endif