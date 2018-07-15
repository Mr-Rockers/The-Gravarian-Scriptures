#include "World.h"
#include "Render.h"

Tile::Tile(TileID id) : id(id), distanceToPlayer(0.0) {}
Tile::Tile() : Tile(TileID_BasicOpaque) {}

glm::ivec4 Tile::renderColour() {
	switch (this->id) {
	case TileID_BasicOpaque:
		return glm::ivec4(0, 0, 0, 255);
	case TileID_BasicTransparent:
		return glm::ivec4(255, 102, 0, 128);
	}
}
bool Tile::isTransparent(){
	switch (this->id) {
	case TileID_BasicOpaque:
		return false;
	case TileID_BasicTransparent:
		return true;
	}
}
bool Tile::allowsOverlap() {
	switch (this->id) {
	case TileID_BasicOpaque:
		return true;
	case TileID_BasicTransparent:
		return false;
	}
}

int WorldInit() {

	for (int x = 0; x < WORLD_MAPSIZE; x++) {
		for (int y = 0; y < WORLD_MAPSIZE; y++) {
			if ((x == 0 || x == WORLD_MAPSIZE - 1) || (y == 0 || y == WORLD_MAPSIZE - 1)) {
				WORLD_MAP[x][y] = new Tile(TileID_BasicOpaque);
			}
		}
	}
	WORLD_MAP[2][2] = new Tile(TileID_BasicTransparent);
	WORLD_MAP[2][3] = new Tile(TileID_BasicTransparent);
	WORLD_MAP[5][4] = new Tile(TileID_BasicTransparent);
	WORLD_MAP[3][4] = new Tile(TileID_BasicTransparent);
	WORLD_MAP[4][4] = new Tile(TileID_BasicTransparent);
	WORLD_MAP[5][4] = new Tile(TileID_BasicTransparent);
	WORLD_MAP[6][4] = new Tile(TileID_BasicTransparent);
	WORLD_MAP[7][4] = new Tile(TileID_BasicTransparent);
	WORLD_MAP[8][4] = new Tile(TileID_BasicTransparent);
	WORLD_MAP[9][4] = new Tile(TileID_BasicTransparent);
	WORLD_MAP[10][4] = new Tile(TileID_BasicTransparent);
	WORLD_MAP[11][4] = new Tile(TileID_BasicTransparent);

	delete WORLD_MAP[2][15];
	WORLD_MAP[2][15] = nullptr;
	return 0;
}

bool WorldUpdate() {
	return true;
}