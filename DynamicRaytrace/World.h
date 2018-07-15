#ifndef WORLD_H
#define WORLD_H

#include "Includes.h"
#include "MathHelper.h"

const unsigned int WORLD_MAPSIZE = 16, WORLD_MAPARRAYSIZE = WORLD_MAPSIZE * WORLD_MAPSIZE;

enum TileID { TileID_BasicOpaque, TileID_BasicTransparent };

struct Tile {
	TileID id;
	double distanceToPlayer; //Only used during rendering.

	Tile();
	Tile(TileID id);

	glm::ivec4 renderColour();
	bool isTransparent();
	bool allowsOverlap();
};
extern Tile* WORLD_MAP[WORLD_MAPSIZE][WORLD_MAPSIZE];
extern Tile TILE_BASIC_OPAQUE, TILE_BASIC_TRANSPARENT;

int WorldInit();
bool WorldUpdate();

#endif