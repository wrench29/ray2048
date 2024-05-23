#include "window.h"

#define CENTERED_ELEMENT_START(screenWidth, elementWidth) (screenWidth - elementWidth) / 2

#define COLOR(red, green, blue) Color{ .r = red, .g = green, .b = blue, .a = 255 }

#define COLOR_TILE_EMPTY COLOR(198, 198, 198) // light gray
#define COLOR_TILE_2     COLOR(255, 153, 153) // very light red
#define COLOR_TILE_4     COLOR(255, 153, 204) // very light pink
#define COLOR_TILE_8     COLOR(153, 153, 255) // light blue
#define COLOR_TILE_16    COLOR(153, 204, 153) // light cyan
#define COLOR_TILE_32    COLOR(255, 102, 178) // light magenta
#define COLOR_TILE_64    COLOR(153, 255, 51)  // lime green
#define COLOR_TILE_128   COLOR(51, 153, 255)  // cyan blue
#define COLOR_TILE_256   COLOR(255, 51, 51)   // red
#define COLOR_TILE_512   COLOR(255, 128, 0)   // orange
#define COLOR_TILE_1024  COLOR(0, 153, 0)     // dark green
#define COLOR_TILE_2048  COLOR(153, 0, 153)   // dark magenta
#define COLOR_TILE_4096  COLOR(0, 0, 204)     // dark blue
#define COLOR_TILE_8192  COLOR(102, 0, 0)     // dark red


Vector2 getTextSize(std::string text) {
    return MeasureTextEx(GetFontDefault(), text.c_str(), kFontSize, 5);
}

void drawText(std::string text, Vector2 position) {
    DrawText(text.c_str(), (int)position.x, (int)position.y, kFontSize, BLACK);
}

GameWindow::GameWindow(): currentScreen(nullptr), forcedClose(false) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(kFramerate);
    InitWindow(kWindowWidth, kWindowHeight, "The 2048 Game");
    SetExitKey(KEY_NULL);
}

bool GameWindow::shouldBeClosed() const {
    return WindowShouldClose() || forcedClose;
}

void GameWindow::updateLogic() {
    if (currentScreen != nullptr) {
        currentScreen->process();
    }
}

void GameWindow::drawFrame() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    if (currentScreen != nullptr) {
        currentScreen->draw();
    }
    EndDrawing();
}

void GameWindow::askToClose() {
    forcedClose = true;
}

GameWindow::~GameWindow() {
    CloseWindow();
}

MainMenuGUI::MainMenuGUI() {
    Vector2 buttonSize = { .x = 250, .y = 50 };
    float buttonLeftOffset = CENTERED_ELEMENT_START(kWindowWidth, buttonSize.x);

    Vector2 playButtonPosition = { .x = buttonLeftOffset, .y = 320 };
    Vector2 settingsButtonPosition = { .x = buttonLeftOffset, .y = playButtonPosition.y + 70 };
    Vector2 exitButtonPosition = { .x = buttonLeftOffset, .y = settingsButtonPosition.y + 70 };
    
    playButton.setText("PLAY");
    playButton.setPosition(playButtonPosition);
    playButton.setSize(buttonSize);

    settingsButton.setText("SETTINGS");
    settingsButton.setPosition(settingsButtonPosition);
    settingsButton.setSize(buttonSize);

    exitButton.setText("EXIT");
    exitButton.setPosition(exitButtonPosition);
    exitButton.setSize(buttonSize);

    logoTextPosition = { .x = CENTERED_ELEMENT_START(kWindowWidth, getTextSize(logoText).x), .y = 150 };
}

void MainMenuGUI::draw() {
    drawText(logoText, logoTextPosition);
    playButton.draw();
    settingsButton.draw();
    exitButton.draw();
}

void MainMenuGUI::process() {
    playButton.process();
    settingsButton.process();
    exitButton.process();
}

bool MainMenuGUI::isPlayButtonClicked() const {
    return playButton.getIsClicked();
}

bool MainMenuGUI::isSettingsButtonClicked() const {
    return settingsButton.getIsClicked();
}

bool MainMenuGUI::isExitButtonClicked() const {
    return exitButton.getIsClicked();
}

SettingsGUI::SettingsGUI() {
    Vector2 textSize = getTextSize(screenText);
    screenTextPosition = {
        .x = CENTERED_ELEMENT_START(kWindowWidth, textSize.x),
        .y = CENTERED_ELEMENT_START(kWindowHeight, textSize.y)
    };
    Vector2 backButtonPosition = { .x = 25, .y = 25 };
    Vector2 backButtonSize = { .x = 200, .y = 50 };
    
    backButton.setText("<- BACK");
    backButton.setPosition(backButtonPosition);
    backButton.setSize(backButtonSize);
}

void SettingsGUI::draw() {
    drawText(screenText, screenTextPosition);
    backButton.draw();
}

void SettingsGUI::process() {
    backButton.process();
}

bool SettingsGUI::isBackButtonClicked() const {
    return backButton.getIsClicked();
}

GameGUI::GameGUI() : tiles{}, isGameFailed(false), isResetAsked(false), score(0) {
    Vector2 backButtonPosition = { .x = 25, .y = 25 };
    Vector2 backButtonSize = { .x = 200, .y = 50 };
    Vector2 resetButtonSize = { .x = 250, .y = 50 };
    Vector2 resetButtonPosition = { 
        .x = CENTERED_ELEMENT_START(kWindowWidth, resetButtonSize.x), 
        .y = kWindowHeight - 75
    };
    
    backButton.setText("<- BACK");
    backButton.setPosition(backButtonPosition);
    backButton.setSize(backButtonSize);

    resetButton.setText("RESET");
    resetButton.setPosition(resetButtonPosition);
    resetButton.setSize(resetButtonSize);

    Vector2 gameFailedTextSize = MeasureTextEx(GetFontDefault(),
        gameFailedText.c_str(), kFontSize, 3);
    gameFailedTextPosition = {
        .x = CENTERED_ELEMENT_START(kWindowWidth, gameFailedTextSize.x),
        .y = kWindowHeight - 100 - gameFailedTextSize.y,
    };
    scoreTextPosition = {
        .x = 300,
        .y = 30
    };
    gameFieldPosition = {
        .x = CENTERED_ELEMENT_START(kWindowWidth, kFieldSize),
        .y = CENTERED_ELEMENT_START(kWindowHeight, kFieldSize) - 40,
    };
}

bool GameGUI::getIsResetAsked() {
    bool temp = isResetAsked;
    isResetAsked = false;
    return temp;
}

void GameGUI::setGameFailed() {
    isGameFailed = true;
}

void GameGUI::reset() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            tiles[y][x] = GameTileType::NoTile;
        }
    }
    pendingTiles.clear();
    animations.clear();
    isGameFailed = false;
    score = 0;
}

std::string GameGUI::getScoreText() {
    std::ostringstream scoreBuilder;
    scoreBuilder << scoreText << score;
    return scoreBuilder.str();
}

void GameGUI::draw() {
    backButton.draw();
    resetButton.draw();
    Rectangle mainFieldBackground{
        .x = gameFieldPosition.x,
        .y = gameFieldPosition.y,
        .width = kFieldSize, 
        .height = kFieldSize,
    };
    DrawRectangleRounded(mainFieldBackground, 0.05f, 20, COLOR(160, 160, 160));
    std::vector<TileWithAbsolutePosition> tilesToDraw = getCurrentTiles();
    for (auto& tile : tilesToDraw) {
        Rectangle tileRectangle{
            .x = tile.x,
            .y = tile.y,
            .width = kTileSize,
            .height = kTileSize,
        };
        Color tileColor = getTileColor(tile.tileType);
        DrawRectangleRounded(tileRectangle, 0.3f, 5, tileColor);
        std::string tileText = getTileText(tile.tileType);
        if (tileText.size() != 0) {
            Vector2 textSize = MeasureTextEx(GetFontDefault(), 
                                             tileText.c_str(), kFontSize + 8, 
                                             3);
            Vector2 textPosition{
                .x = (kTileSize - textSize.x) / 2 + tileRectangle.x,
                .y = (kTileSize - textSize.y) / 2 + tileRectangle.y,
            };
            DrawText(tileText.c_str(), (int)textPosition.x, 
                     (int)textPosition.y, kFontSize + 8, WHITE);
        }
    }
    DrawText(getScoreText().c_str(), (int)scoreTextPosition.x,
        (int)scoreTextPosition.y, kFontSize, BLACK);
    if (!isGameFailed) {
        return;
    }
    DrawText(gameFailedText.c_str(), (int)gameFailedTextPosition.x,
             (int)gameFailedTextPosition.y, kFontSize, BLACK);
}

void GameGUI::process() {
    backButton.process();
    resetButton.process();
    if (!isResetAsked && resetButton.getIsClicked()) {
        isResetAsked = true;
    }
    for (int i = ((int)animations.size() - 1); i >= 0; i--) {
        auto& tileAnimation = animations[i];
        if (tileAnimation.currentStep == (kTileAnimationSteps - 1)) {
            if (tileAnimation.newTile != GameTileType::NoTile) {
                tiles[tileAnimation.toY][tileAnimation.toX] = 
                    tileAnimation.newTile;
            }
            animations.erase(std::next(animations.begin(), i));
            continue;
        }
        tileAnimation.currentStep++;
    }
    if (animations.size() == 0) {
        for (auto& pendingTile : pendingTiles) {
            tiles[pendingTile.y][pendingTile.x] = pendingTile.tileType;
        }
        pendingTiles.clear();
    }
}

bool GameGUI::isBackButtonClicked() const {
    return backButton.getIsClicked();
}

void GameGUI::updateScore(int newScore) {
    score = newScore;
}

void GameGUI::setTile(int x, int y, GameTileType tileType) {
    if (x < 0 || x > 3 || y < 0 || y > 3) {
        return;
    }
    pendingTiles.push_back(TileWithPosition{
        .x = x,
        .y = y,
        .tileType = tileType
    });
}

void GameGUI::moveTile(int fromX, int fromY, int toX, int toY, 
                       GameTileType oldTile, GameTileType newTile) {
    if (fromX < 0 || fromX > 3 || toX < 0 || toX > 3 ||
        fromY < 0 || fromY > 3 || toY < 0 || toY > 3) {
        return;
    }
    if (tiles[fromY][fromX] == GameTileType::NoTile) {
        return;
    }
    animations.push_back(TileMovementAnimation{
        .fromX = fromX,
        .fromY = fromY,
        .toX = toX,
        .toY = toY,
        .currentStep = 0,
        .oldTile = oldTile,
        .newTile = newTile,
    });
    tiles[fromY][fromX] = GameTileType::NoTile;
}

UserMovement GameGUI::getUserMovement() {
    if (!animations.empty()) {
        return UserMovement::None;
    }
    if (IsKeyReleased(KEY_W) || IsKeyReleased(KEY_UP)) {
        return UserMovement::Up;
    }
    if (IsKeyReleased(KEY_A) || IsKeyReleased(KEY_LEFT)) {
        return UserMovement::Left;
    }
    if (IsKeyReleased(KEY_S) || IsKeyReleased(KEY_DOWN)) {
        return UserMovement::Down;
    }
    if (IsKeyReleased(KEY_D) || IsKeyReleased(KEY_RIGHT)) {
        return UserMovement::Right;
    }
    return UserMovement::None;
}

Color GameGUI::getTileColor(GameTileType tileType) {
    switch (tileType) {
    case GameTileType::Tile2:
        return COLOR_TILE_2;
    case GameTileType::Tile4:
        return COLOR_TILE_4;
    case GameTileType::Tile8:
        return COLOR_TILE_8;
    case GameTileType::Tile16:
        return COLOR_TILE_16;
    case GameTileType::Tile32:
        return COLOR_TILE_32;
    case GameTileType::Tile64:
        return COLOR_TILE_64;
    case GameTileType::Tile128:
        return COLOR_TILE_128;
    case GameTileType::Tile256:
        return COLOR_TILE_256;
    case GameTileType::Tile512:
        return COLOR_TILE_512;
    case GameTileType::Tile1024:
        return COLOR_TILE_1024;
    case GameTileType::Tile2048:
        return COLOR_TILE_2048;
    case GameTileType::Tile4096:
        return COLOR_TILE_4096;
    case GameTileType::Tile8192:
        return COLOR_TILE_8192;
    }
    return COLOR_TILE_EMPTY;
}

std::string GameGUI::getTileText(GameTileType tileType) {
    switch (tileType) {
    case GameTileType::Tile2:
        return "2";
    case GameTileType::Tile4:
        return "4";
    case GameTileType::Tile8:
        return "8";
    case GameTileType::Tile16:
        return "16";
    case GameTileType::Tile32:
        return "32";
    case GameTileType::Tile64:
        return "64";
    case GameTileType::Tile128:
        return "128";
    case GameTileType::Tile256:
        return "256";
    case GameTileType::Tile512:
        return "512";
    case GameTileType::Tile1024:
        return "1024";
    case GameTileType::Tile2048:
        return "2048";
    case GameTileType::Tile4096:
        return "4096";
    case GameTileType::Tile8192:
        return "8192";
    }
    return "";
}

Vector2 GameGUI::calculateTilePosition(int x, int y) {
    return Vector2{
        .x = gameFieldPosition.x + kGapSize + ((kGapSize + kTileSize) * x),
        .y = gameFieldPosition.y + kGapSize + ((kGapSize + kTileSize) * y),
    };
}

std::vector<TileWithAbsolutePosition> GameGUI::getCurrentTiles() {
    std::vector<TileWithAbsolutePosition> tilesToDraw;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            Vector2 tilePosition = calculateTilePosition(x, y);
            tilesToDraw.push_back(TileWithAbsolutePosition{
                .x = tilePosition.x,
                .y = tilePosition.y,
                .tileType = tiles[y][x],
            });
        }
    }
    for (auto& tileAnimation : animations) {
        Vector2 tileOldPosition = calculateTilePosition(tileAnimation.fromX, 
                                                        tileAnimation.fromY);
        Vector2 tileNewPosition = calculateTilePosition(tileAnimation.toX,
                                                        tileAnimation.toY);
        float distanceX = tileNewPosition.x - tileOldPosition.x;
        float distanceY = tileNewPosition.y - tileOldPosition.y;
        Vector2 tileCurrentPosition{
            .x = tileOldPosition.x,
            .y = tileOldPosition.y,
        };
        if (distanceX >= 1 || distanceX <= -1) {
            tileCurrentPosition.x += ((distanceX / kTileAnimationSteps) * 
                                      tileAnimation.currentStep);
        }
        if (distanceY >= 1 || distanceY <= -1) {
            tileCurrentPosition.y += ((distanceY / kTileAnimationSteps) * 
                                      tileAnimation.currentStep);
        }
        tilesToDraw.push_back(TileWithAbsolutePosition{
            .x = tileCurrentPosition.x,
            .y = tileCurrentPosition.y,
            .tileType = tileAnimation.oldTile,
        });
    }
    return tilesToDraw;
}
