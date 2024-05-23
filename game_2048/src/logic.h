#ifndef GAME_2048_LOGIC_H
#define GAME_2048_LOGIC_H

#include <vector>
#include <random>

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

struct FieldLine {
	GameTileType line[4];
};

class GameField {
private:
	GameTileType tiles[4][4];
	bool isInitialized;

	int score;

	std::mt19937 randomGenerator;

	int randomNumber(int min, int max);

	std::vector<TileWithPosition> getEmptyTiles();

	std::vector<TileLineMovement> moveLine(FieldLine& line, bool reversed);

	std::vector<TileMovement> verticalMove(bool reversed, bool modifyField);
	std::vector<TileMovement> horizontalMove(bool reversed, bool modifyField);

public:
	GameField();

	std::vector<TileWithPosition> spawnNewTiles();
	std::vector<TileMovement> requestMovement(UserMovement movement);
	bool isGameFailed();
	bool isGameInitialized() const { return isInitialized; }
    void reset();
	int getScore() const { return score; }
};

#endif // GAME_2048_LOGIC_H
