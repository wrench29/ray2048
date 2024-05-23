#ifndef GAME_2048_WINDOW_H
#define GAME_2048_WINDOW_H

#include <string>
#include <vector>
#include <sstream>

#include <raylib.h>

#include "widgets.h"

const int kFontSize = 40;

const unsigned int kWindowWidth = 1000;
const unsigned int kWindowHeight = 900;

const int kFramerate = 144;

const int kTileAnimationSteps = 20;

enum class GameTileType {
	NoTile = 0,
	Tile2,
	Tile4,
	Tile8,
	Tile16,
	Tile32,
	Tile64,
	Tile128,
	Tile256,
	Tile512,
	Tile1024,
	Tile2048,
	Tile4096,
	Tile8192,
};

enum class UserMovement {
	None,
	Left,
	Right,
	Up,
	Down,
};

struct TileMovementAnimation {
	int fromX;
	int fromY;
	int toX;
	int toY;
	int currentStep;
	GameTileType oldTile;
	GameTileType newTile;
};

struct TileWithAbsolutePosition {
	float x;
	float y;
	GameTileType tileType;
};

struct TileWithPosition {
	int x;
	int y;
	GameTileType tileType;
};

class IGUIScreen {
public:	
	virtual void draw() = 0;
	virtual void process() = 0;
};

class MainMenuGUI : public IGUIScreen {
private:
	const std::string logoText = "The 2048 Game";

	Vector2 logoTextPosition;

	Button playButton;
	Button settingsButton;
	Button exitButton;

public:
	MainMenuGUI();

	virtual void draw();
	virtual void process();

	bool isPlayButtonClicked() const;
	bool isSettingsButtonClicked() const;
	bool isExitButtonClicked() const;
};

class SettingsGUI : public IGUIScreen {
private:
	const std::string screenText = "Currently there are no settings.";

	Vector2 screenTextPosition;

	Button backButton;

public:
	SettingsGUI();

	virtual void draw();
	virtual void process();

	bool isBackButtonClicked() const;
};

class GameGUI : public IGUIScreen {
private:
	const float kTileSize = 128;
	const float kGapSize = 16;
	const float kFieldSize = (kTileSize * 4) + (kGapSize * 5);

    const std::string gameFailedText = 
        "You lose :( Press \"RESET\" to try again.";
	const std::string scoreText = "Score: ";

	Button backButton;
	Button resetButton;

	Vector2 gameFailedTextPosition;
	Vector2 scoreTextPosition;

	Vector2 gameFieldPosition;

    bool isGameFailed;
    bool isResetAsked;

	int score;

	GameTileType tiles[4][4]; // usage: tiles[0][2] where 0 - Y, 2 - X. 
	std::vector<TileMovementAnimation> animations;
	std::vector<TileWithPosition> pendingTiles;

	std::string getScoreText();
	Vector2 calculateTilePosition(int x, int y);
	Color getTileColor(GameTileType tileType);
	std::string getTileText(GameTileType tileType);
	std::vector<TileWithAbsolutePosition> getCurrentTiles();
	void drawTile(TileWithAbsolutePosition tile);

public:
	GameGUI();

	virtual void draw();
	virtual void process();

	bool isBackButtonClicked() const;

	void setTile(int x, int y, GameTileType tileType);
	void moveTile(int fromX, int fromY, int toX, int toY, 
                  GameTileType oldTile, GameTileType newTile);
	void updateScore(int newScore);
    bool getIsResetAsked();
    void setGameFailed();
	void reset();
    UserMovement getUserMovement();
};

class GameWindow {
private:
	IGUIScreen* currentScreen;

	bool forcedClose;

public:
	GameWindow();
	~GameWindow();

	bool shouldBeClosed() const;
	void askToClose();
	void updateLogic();
	void drawFrame();

	void setCurrentScreen(IGUIScreen* screen) { currentScreen = screen; }
};

#endif // GAME_2048_WINDOW_H
