#include "logic.h"

GameField::GameField() : tiles{}, isInitialized(false), score(0) {
	std::random_device dev;
	randomGenerator = std::mt19937(dev());
}

// Generate number in range [min; max]
int GameField::randomNumber(int min, int max) {
	std::uniform_int_distribution<std::mt19937::result_type> distribution(min, max);
	return distribution(randomGenerator);
}

void GameField::reset() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            tiles[y][x] = GameTileType::NoTile;
        }
    }
	score = 0;
    isInitialized = false;
}

std::vector<TileWithPosition> GameField::spawnNewTiles() {
	std::vector<TileWithPosition> newTiles;
	auto emptyTiles = getEmptyTiles();
	if (emptyTiles.size() == 0) {
		return newTiles;
	}
	int countOfTilesToSpawn = isInitialized ? 1 : 2;
	for (int i = 0; i < countOfTilesToSpawn; i++) {
		bool is4Tile = randomNumber(1, 10) > 8; // generate by random with 10% chance
		GameTileType tileToSpawn = is4Tile ? GameTileType::Tile4 : GameTileType::Tile2;
		bool isReallyEmpty = false;
		int randomIndex = 0;
		while (!isReallyEmpty) {
			if (emptyTiles.size() == 0) {
				return newTiles;
			}
			randomIndex = randomNumber(0, emptyTiles.size() - 1);
			if (tiles[emptyTiles[randomIndex].y][emptyTiles[randomIndex].x] == GameTileType::NoTile) {
				isReallyEmpty = true;
				break;
			}
			emptyTiles.erase(std::next(emptyTiles.begin(), randomIndex));
		}
		int emptyX = emptyTiles[randomIndex].x;
		int emptyY = emptyTiles[randomIndex].y;
		tiles[emptyY][emptyX] = tileToSpawn;
		newTiles.push_back(TileWithPosition{
			.x = emptyX,
			.y = emptyY,
			.tileType = tileToSpawn,
		});
	}
	if (!isInitialized) {
		isInitialized = true;
	}
	return newTiles;
}

std::vector<TileWithPosition> GameField::getEmptyTiles() {
	std::vector<TileWithPosition> emptyTiles;
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (tiles[y][x] == GameTileType::NoTile) {
				emptyTiles.push_back(TileWithPosition{
					.x = x,
					.y = y,
					.tileType = GameTileType::NoTile,
				});
			}
		}
	}
	return emptyTiles;
}

std::vector<TileMovement> GameField::requestMovement(UserMovement movement) {
	std::vector<TileMovement> movedTiles;
	switch (movement) {
	case UserMovement::Left: {
		movedTiles = horizontalMove(false, true);
		break;
	}
	case UserMovement::Right: {
		movedTiles = horizontalMove(true, true);
		break;
	}
	case UserMovement::Up: {
		movedTiles = verticalMove(false, true);
		break;
	}
	case UserMovement::Down: {
		movedTiles = verticalMove(true, true);
		break;
	}
	}
	std::vector<TileMovement> uniqueMoves;
	for (int i = 0; i < movedTiles.size(); i++) {
		bool isUnique = true;
		for (int j = 0; j < uniqueMoves.size(); j++) {
			if (movedTiles[i].toX == uniqueMoves[j].toX && movedTiles[i].toY == uniqueMoves[j].toY) {
				isUnique = false;
				break;
			}
		}
		if (isUnique) {
			uniqueMoves.push_back(movedTiles[i]);
		}
	}
	for (auto& tile : uniqueMoves) {
		int oldTileValue = (int)tile.oldTile;
		int newTileValue = (int)tile.newTile;
		if (oldTileValue != 0 && oldTileValue != newTileValue) {
			int scoreUp = pow(2, newTileValue);
			score += scoreUp;
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

// Move tiles in left direction, IF reversed == true, move in right direction
std::vector<TileLineMovement> GameField::moveLine(FieldLine& line, bool reversed) {
	std::vector<TileLineMovement> movedTiles;
	bool wasTileMerged[4]{false};
	int i = reversed ? 3 : 0;
	int stopI = reversed ? 0 : 3;
	bool endExprI = reversed ? i < stopI : i > stopI;
	while (!endExprI) {
		// skip empty tiles
		if (line.line[i] == GameTileType::NoTile) {
			i += reversed ? -1 : 1;
			endExprI = reversed ? i < stopI : i > stopI;
			continue;
		}
		// prepare movements
		GameTileType upgradeTile = (GameTileType)((int)line.line[i] + 1);
		TileLineMovement tileMovement{
			.from = i,
			.to = -1,
			.oldTile = line.line[i],
			.newTile = line.line[i],
		};
		TileLineMovement mergedTileMovement{
			.from = -1,
			.to = -1,
			.oldTile = line.line[i],
			.newTile = upgradeTile,
		};
		// check if there is available movement for tile 'i'
		int lastAvailableSpot = i;
		int j = reversed ? (i + 1) : (i - 1);
		int stopJ = reversed ? 3 : 0;
		bool endExpr = reversed ? j > stopJ : j < stopJ;
		while (!endExpr) {
			if (line.line[j] == GameTileType::NoTile) {
				lastAvailableSpot = j;
			}
			else if (line.line[j] == line.line[i] && !wasTileMerged[j]) {
				lastAvailableSpot = i;
				break;
			}
			else {
				break;
			}
			j += reversed ? 1 : -1;
			endExpr = reversed ? j > stopJ : j < stopJ;
		}
		// if there is available movement - make it
		if (lastAvailableSpot != i) {
			tileMovement.to = lastAvailableSpot;
			line.line[lastAvailableSpot] = line.line[i];
			line.line[i] = GameTileType::NoTile;
		}
		// check if there is available merge for tile 'lastAvailableMovement'
		int tileToMerge = -1;
		j = reversed ? (lastAvailableSpot - 1) : (lastAvailableSpot + 1);
		stopJ = reversed ? 0 : 3;
		endExpr = reversed ? j < stopJ : j > stopJ;
		while (!endExpr) {
			if (line.line[j] == tileMovement.oldTile) {
				tileToMerge = j;
				break;
			}
			if (line.line[j] != GameTileType::NoTile && line.line[j] != tileMovement.oldTile) {
				break;
			}
			j += reversed ? -1 : 1;
			endExpr = reversed ? j < stopJ : j > stopJ;
		}
		// if merge is available - make it
		if (tileToMerge != -1) {
			mergedTileMovement.from = tileToMerge;
			mergedTileMovement.to = lastAvailableSpot;
			tileMovement.newTile = mergedTileMovement.newTile;
			line.line[lastAvailableSpot] = mergedTileMovement.newTile;
			line.line[tileToMerge] = GameTileType::NoTile;
			wasTileMerged[lastAvailableSpot] = true;
		}
		// if movements were made - add them to the vector
		if (tileMovement.to != -1) {
			movedTiles.push_back(tileMovement);
		}
		if (mergedTileMovement.to != -1) {
			movedTiles.push_back(mergedTileMovement);
		}
		i += reversed ? -1 : 1;
		endExprI = reversed ? i < stopI : i > stopI;
	}
	return movedTiles;
}

std::vector<TileMovement> GameField::horizontalMove(bool reversed, bool modifyField) {
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
		if (modifyField) {
			tiles[y][0] = line.line[0];
			tiles[y][1] = line.line[1];
			tiles[y][2] = line.line[2];
			tiles[y][3] = line.line[3];
		}
	}
	return movedTiles;
}

std::vector<TileMovement> GameField::verticalMove(bool reversed, bool modifyField) {
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
		if (modifyField) {
			tiles[0][x] = line.line[0];
			tiles[1][x] = line.line[1];
			tiles[2][x] = line.line[2];
			tiles[3][x] = line.line[3];
		}
	}
	return movedTiles;
}

bool GameField::isGameFailed() {
	bool leftMoveAvailable = horizontalMove(true, false).size() > 0;
	bool rightMoveAvailable = horizontalMove(false, false).size() > 0;
	bool upMoveAvailable = verticalMove(true, false).size() > 0;
	bool downMoveAvailable = verticalMove(false, false).size() > 0;
	return !leftMoveAvailable && !rightMoveAvailable && 
           !upMoveAvailable && !downMoveAvailable;
}

