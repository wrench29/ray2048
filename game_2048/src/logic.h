#ifndef GAME_2048_LOGIC_H
#define GAME_2048_LOGIC_H

#include <vector>

#include "window.h"

struct TileMovement {
	int fromX;
	int fromY;
	int toX;
	int toY;
	GameTileType tileType;
};

struct TileWithPosition {
	int x;
	int y;
	GameTileType tileType;
};

class GameField {
private:
	GameTileType tiles[4][4];
	bool isInitialized;

public:
	GameField();

	std::vector<TileWithPosition> spawnNewTiles();
	std::vector<TileMovement> requestMovement(UserMovement movement);
	bool isGameFailed();
	bool isGameInitialized() const { return isInitialized; }
};

#endif // GAME_2048_LOGIC_H
