#ifndef GAME_2048_LOGIC_H
#define GAME_2048_LOGIC_H

#include <vector>

#include "window.h"

struct TileMovement {
	int fromX;
	int fromY;
	int toX;
	int toY;
	GameTileType oldTile;
	GameTileType newTile;
};

struct TileLineMovement {
	int from;
	int to;
	GameTileType oldTile;
	GameTileType newTile;
};

struct TileWithPosition {
	int x;
	int y;
	GameTileType tileType;
};

struct FieldLine {
	GameTileType line[4];
};

class GameField {
private:
	GameTileType tiles[4][4];
	bool isInitialized;

	std::vector<TileLineMovement> moveLine(FieldLine line, bool reversed);

	std::vector<TileMovement> verticalMove(bool reversed);
	std::vector<TileMovement> horizontalMove(bool reversed);

public:
	GameField();

	std::vector<TileWithPosition> spawnNewTiles();
	std::vector<TileMovement> requestMovement(UserMovement movement);
	bool isGameFailed();
	bool isGameInitialized() const { return isInitialized; }
};

#endif // GAME_2048_LOGIC_H
