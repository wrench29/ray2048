#ifndef GAME_2048_WINDOW_H
#define GAME_2048_WINDOW_H

#include <string>
#include <vector>

#include <raylib.h>

const int kFontSize = 40;

const unsigned int kWindowWidth = 1280;
const unsigned int kWindowHeight = 720;

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

class IGUIScreen {
public:	
	virtual void draw() = 0;
	virtual void process() = 0;
};

class MainMenuGUI : public IGUIScreen {
private:
	const std::string logoText = "The 2048 Game";
	const std::string playButtonText = "PLAY";
	const std::string settingsButtonText = "SETTINGS";
	const std::string exitButtonText = "EXIT";

	Vector2 logoTextPosition;

	Vector2 playButtonPosition;
	Vector2 playButtonSize;
	
	Vector2 settingsButtonPosition;
	Vector2 settingsButtonSize;

	Vector2 exitButtonPosition;
	Vector2 exitButtonSize;

	bool isPlayButtonHovered;
	bool isSettingsButtonHovered;
	bool isExitButtonHovered;

	Color hoveredButtonColor;

	bool isCursorPointing;

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
	const std::string backButtonText = "<- Back";

	Vector2 screenTextPosition;

	Vector2 backButtonPosition;
	Vector2 backButtonSize;

	bool isBackButtonHovered;

	Color hoveredButtonColor;

	bool isCursorPointing;

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

	const std::string backButtonText = "<- Back";

	Vector2 backButtonPosition;
	Vector2 backButtonSize;

	bool isBackButtonHovered;

	Color hoveredButtonColor;

	bool isCursorPointing;

	GameTileType tiles[4][4]; // usage: tiles[0][2] where 0 - Y, 2 - X. 
	std::vector<TileMovementAnimation> animations;

	Vector2 calculateTilePosition(int x, int y);
	Color getTileColor(GameTileType tileType);
	std::string getTileText(GameTileType tileType);
	std::vector<TileWithAbsolutePosition> getCurrentTiles();

public:
	GameGUI();

	virtual void draw();
	virtual void process();

	bool isBackButtonClicked() const;

	void setTile(int x, int y, GameTileType tileType);
	void moveTile(int fromX, int fromY, int toX, int toY, GameTileType oldTile, GameTileType newTile);
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
