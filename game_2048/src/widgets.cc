#include "widgets.h"

#define HOVERED_BUTTON_COLOR { .r = 230, .g = 100, .b = 100, .a = 255 }

#define FONT_SIZE 40

Button::Button(std::string text, Vector2 position, Vector2 size) :
	text(text), position(position), size(size), normalColor(RED),
	hoveredColor(HOVERED_BUTTON_COLOR), isClicked(false), isHovered(false),
    isCursorChanged(false) {}

void Button::process() {
    Vector2 mousePosition = GetMousePosition();
    isHovered = mousePosition.x >= position.x && mousePosition.x <= (position.x + size.x) &&
        mousePosition.y >= position.y && mousePosition.y <= (position.y + size.y);
    isClicked = isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    if (isHovered && !isCursorChanged) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        isCursorChanged = true;
    }
    if (isClicked || (!isHovered && isCursorChanged)) {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        isCursorChanged = false;
    }
}

void Button::draw() {
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text.c_str(), FONT_SIZE, 3);
    Vector2 textPosition{
        .x = (size.x - textSize.x) / 2 + position.x,
        .y = (size.y - textSize.y) / 2 + position.y,
    };
    Rectangle rectangle = {
        .x = position.x,
        .y = position.y,
        .width = size.x,
        .height = size.y,
    };
    Color color = isHovered ? hoveredColor : normalColor;
    DrawRectangleRounded(rectangle, 0.3f, 5, color);
    DrawText(text.c_str(), (int)textPosition.x, (int)textPosition.y, FONT_SIZE, WHITE);
}

bool Button::getIsClicked() const {
    return isClicked;
}
