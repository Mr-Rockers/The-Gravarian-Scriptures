#ifndef WORLD_H
#define WORLD_H

#include "Includes.h"
#include "MathHelper.h"

const unsigned int WORLD_MAPSIZE = 16, WORLD_MAPARRAYSIZE = WORLD_MAPSIZE * WORLD_MAPSIZE;
const double WORLD_GRAVITATIONALPULL = -9.8, WORLD_HEIGHT = 0.0; //Placeholder values until proper map structure is implemented.

enum TileID { TileID_BasicOpaque, TileID_BasicTransparent };

struct Tile {
	double distanceToPlayer; //Only used during rendering.
	int coordX, coordY; //Only used during rendering.

	TileID id;

	Tile();
	Tile(TileID id);

	glm::ivec4 renderColour();
	bool isTransparent(); //Is this tile transparent?
	bool allowsOverlap(); //NEEDS IMPLEMENTATION. Can other tiles of the same type render through this?
	bool isSolid(); //Should the engine prevent the player walking through this?
	bool hasFloor(); //Should the engine prevent the player falling through this?
};
extern Tile* WORLD_MAP[WORLD_MAPSIZE][WORLD_MAPSIZE];
extern Tile TILE_BASIC_OPAQUE, TILE_BASIC_TRANSPARENT;

int WorldInit();
bool WorldUpdate();

#endif