#ifndef GAME_2048_WIDGETS_H
#define GAME_2048_WIDGETS_H

#include <string>

#include <raylib.h>

class Button {
private:
	std::string text; 
	Vector2 position; 
	Vector2 size;
	Color normalColor;
	Color hoveredColor;
	bool isClicked;
	bool isHovered;
	bool isCursorChanged;

public:
	Button(std::string text, Vector2 position, Vector2 size);
	Button() : Button("", {}, {}) {}

	void setText(std::string text) { this->text = text; }
	void setPosition(Vector2 position) { this->position = position; }
	void setSize(Vector2 size) { this->size = size; }

	void process();
	void draw();
	bool getIsClicked() const;
};

#endif // GAME_2048_WIDGETS_H
