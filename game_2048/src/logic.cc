#include "logic.h"

GameField::GameField() : tiles{}, isInitialized(false) {
	
}

std::vector<TileWithPosition> GameField::spawnNewTiles() {
	std::vector<TileWithPosition> newTiles;
	if (!isInitialized) {
		isInitialized = true;
		tiles[0][0] = GameTileType::Tile4;
		tiles[1][1] = GameTileType::Tile4;
		tiles[2][2] = GameTileType::Tile2;
		tiles[3][3] = GameTileType::Tile4;
		newTiles.push_back(TileWithPosition{
			.x = 0,
			.y = 0,
			.tileType = GameTileType::Tile4,
		});
		newTiles.push_back(TileWithPosition{
			.x = 1,
			.y = 1,
			.tileType = GameTileType::Tile4,
		});
		newTiles.push_back(TileWithPosition{
			.x = 2,
			.y = 2,
			.tileType = GameTileType::Tile2,
			});
		newTiles.push_back(TileWithPosition{
			.x = 3,
			.y = 3,
			.tileType = GameTileType::Tile4,
			});
	}
	return newTiles;
}

std::vector<TileMovement> GameField::requestMovement(UserMovement movement) {
	std::vector<TileMovement> movedTiles;
	switch (movement) {
	case UserMovement::Left: {
		movedTiles = horizontalMove(true);
		break;
	}
	case UserMovement::Right: {
		movedTiles = horizontalMove(false);
		break;
	}
	case UserMovement::Up: {
		movedTiles = verticalMove(true);
		break;
	}
	case UserMovement::Down: {
		movedTiles = verticalMove(false);
		break;
	}
	}
	if (movedTiles.size() > 0) {
		for (auto& tile : movedTiles) {
			tiles[tile.fromY][tile.fromX] = GameTileType::NoTile;
			tiles[tile.toY][tile.toX] = tile.newTile;
		}
	}
	return movedTiles;
}

/*
	Tile movement logic:
	 - Check if there is space in line, in direction of movement;
	 - Check if there are tiles with same number, that are in sequence.
	If movement is available, then we create movement objects for each
	available movement.
	General rules:
	 - Do all moves in sequence, with processing of every change;
	 - There is one scenario, in which line has 4 tiles with the same number, and we need to count that;
	 - Tile merge can occur ONLY with tiles, that were moved by user (for example -> 2 | 2 | 2 | 2 => X | X | 4 | 4 );
*/

// Move tiles in right direction, IF reversed == true, move in left direction
std::vector<TileLineMovement> GameField::moveLine(FieldLine line, bool reversed) {
	std::vector<TileLineMovement> movedTiles;
	for (int targetTile = 3; targetTile > 0; targetTile--) {
		bool isTargetEmpty = false;
		if (line.line[targetTile] == GameTileType::NoTile) {
			isTargetEmpty = true;
		}
		for (int i = (targetTile - 1); i >= 0; i--) {
			if (line.line[i] == GameTileType::NoTile) {
				continue;
			}
			if (line.line[i] == line.line[targetTile]) {
				// Merge tiles
				GameTileType newTile = (GameTileType)((int)line.line[targetTile] + 1);
				movedTiles.push_back(TileLineMovement{
					.from = i,
					.to = targetTile,
					.oldTile = line.line[targetTile],
					.newTile = newTile,
				});
				line.line[i] = GameTileType::NoTile;
				break;
			}
			if (isTargetEmpty) {
				movedTiles.push_back(TileLineMovement{
					.from = i,
					.to = targetTile,
					.oldTile = line.line[i],
					.newTile = line.line[i],
					});
				line.line[i] = GameTileType::NoTile;
				break;
			}
			if (line.line[i] != line.line[targetTile]) {
				break;
			}
		}
	}
	return movedTiles;
}

std::vector<TileMovement> GameField::horizontalMove(bool reversed) {
	std::vector<TileMovement> movedTiles;
	for (int y = 0; y < 4; y++) {
		FieldLine line{
			.line = {
				tiles[y][0],
				tiles[y][1],
				tiles[y][2],
				tiles[y][3]
			},
		};
		auto newMovedTiles = moveLine(line, reversed);
		for (int i = 0; i < newMovedTiles.size(); i++) {
			TileLineMovement lineMove = newMovedTiles[i];
			movedTiles.push_back(TileMovement{
				.fromX = lineMove.from,
				.fromY = y,
				.toX = lineMove.to,
				.toY = y,
				.oldTile = lineMove.oldTile,
				.newTile = lineMove.newTile,
			});
		}
	}
	return movedTiles;
}

std::vector<TileMovement> GameField::verticalMove(bool reversed) {
	std::vector<TileMovement> movedTiles;
	for (int x = 0; x < 4; x++) {
		FieldLine line{
			.line = {
				tiles[0][x],
				tiles[1][x],
				tiles[2][x],
				tiles[3][x]
			},
		};
		auto newMovedTiles = moveLine(line, reversed);
		for (int i = 0; i < newMovedTiles.size(); i++) {
			TileLineMovement lineMove = newMovedTiles[i];
			movedTiles.push_back(TileMovement{
				.fromX = x,
				.fromY = lineMove.from,
				.toX = x,
				.toY = lineMove.to,
				.oldTile = lineMove.oldTile,
				.newTile = lineMove.newTile,
			});
		}
	}
	return movedTiles;
}

bool GameField::isGameFailed() {
	bool leftMoveAvailable = horizontalMove(false).size() > 0;
	bool rightMoveAvailable = horizontalMove(true).size() > 0;
	bool upMoveAvailable = verticalMove(false).size() > 0;
	bool downMoveAvailable = verticalMove(true).size() > 0;
	return leftMoveAvailable || rightMoveAvailable || upMoveAvailable || downMoveAvailable;
}
