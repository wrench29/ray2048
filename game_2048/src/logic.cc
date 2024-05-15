#include "logic.h"

GameField::GameField() : tiles{}, isInitialized(false) {
	
}

std::vector<TileWithPosition> GameField::spawnNewTiles() {
	if (!isInitialized) {
		isInitialized = true;
	}
	std::vector<TileWithPosition> newTiles;
	return newTiles;
}

std::vector<TileMovement> GameField::requestMovement(UserMovement movement) {
	// Check if movement available, if so - move and return moves, if not - do nothing
	std::vector<TileMovement> movedTiles;
	return movedTiles;
}

bool GameField::isGameFailed() {
	// Are there any available movements?
	return true;
}
