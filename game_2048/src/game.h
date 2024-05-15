#ifndef GAME_2048_GAME_H
#define GAME_2048_GAME_H

#include "window.h"
#include "logic.h"

enum class GameScreenType {
	MainMenu = 0,
	Settings,
	Game
};

class Game2048 {
private:
	GameWindow window;

	GameScreenType currentScreenType;

	MainMenuGUI mainMenuScreen;
	SettingsGUI settingsScreen;
	GameGUI gameScreen;

	void processMainMenu();
	void processSettings();
	void processGame();

public:
	Game2048();

	void run();
};

#endif // GAME_2048_GAME_H
