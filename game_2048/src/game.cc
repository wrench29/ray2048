#include "game.h"

#include <iostream>

Game2048::Game2048() : currentScreenType(GameScreenType::MainMenu) {
	window.setCurrentScreen(&mainMenuScreen);
	if (!gameField.isGameInitialized()) {
		auto spawnedTiles = gameField.spawnNewTiles();
		for (auto& spawnedTile : spawnedTiles) {
			gameScreen.setTile(spawnedTile.x, spawnedTile.y, spawnedTile.tileType);
		}
	}
}

void Game2048::run() {
	while (!window.shouldBeClosed()) {
		window.updateLogic();
		switch (currentScreenType) {
		case GameScreenType::MainMenu: {
			processMainMenu();
			break;
		}
		case GameScreenType::Settings: {
			processSettings();
			break;
		}
		case GameScreenType::Game: {
			processGame();
			break;
		}
		}
		window.drawFrame();
	}
}

void Game2048::processMainMenu() {
	if (mainMenuScreen.isPlayButtonClicked()) {
		currentScreenType = GameScreenType::Game;
		window.setCurrentScreen(&gameScreen);
	}
	if (mainMenuScreen.isSettingsButtonClicked()) {
		currentScreenType = GameScreenType::Settings;
		window.setCurrentScreen(&settingsScreen);
	}
	if (mainMenuScreen.isExitButtonClicked()) {
		window.askToClose();
	}
}

void Game2048::processSettings() {
	if (settingsScreen.isBackButtonClicked()) {
		currentScreenType = GameScreenType::MainMenu;
		window.setCurrentScreen(&mainMenuScreen);
	}
}

void Game2048::processGame() {
	if (gameScreen.isBackButtonClicked()) {
		currentScreenType = GameScreenType::MainMenu;
		window.setCurrentScreen(&mainMenuScreen);
	}
	UserMovement userMove = gameScreen.getUserMovement();
	if (userMove != UserMovement::None) {
		auto fieldChanges = gameField.requestMovement(userMove);
		if (fieldChanges.size() > 0) {
			for (auto& tileMove : fieldChanges) {
				gameScreen.moveTile(tileMove.fromX, tileMove.fromY, tileMove.toX, tileMove.toY, tileMove.tileType);
			}
			auto spawnedTiles = gameField.spawnNewTiles();
			for (auto& spawnedTile : spawnedTiles) {
				gameScreen.setTile(spawnedTile.x, spawnedTile.y, spawnedTile.tileType);
			}
		}
	}
}
