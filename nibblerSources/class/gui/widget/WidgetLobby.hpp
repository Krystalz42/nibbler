#pragma once

#include "AWidget.hpp"
#include "SpriteColorProperties.hpp"
#include <vector>
#include <array>
#include <deque>
#include "WidgetSnake.hpp"
#include "nibbler.hpp"
#include <cores/Snake.hpp>

#define WIDGET_LOBBY_DIRECTORY_SNAKE_PRESENTATION "snake_presentation"

class Core;

class WidgetLobby : public AWidget {
public:
	WidgetLobby(Core &core);

	~WidgetLobby(void) override = default;

	void addSnake(Snake const &snake, bool isYourSnake = false);

	void addColor(eSprite color, std::string const &name, std::string const &pathColor);

	void render(void) override;

private:
	std::map<eSprite, SpriteColorProperties> mapSprite_;
	std::deque<WidgetSnake> snakeWidget_;
	std::array<Snake, SNAKE_MAX> snakes_;

	void _reload();

	WidgetLobby &operator=(const WidgetLobby &) = delete;

	WidgetLobby(const WidgetLobby &) = delete;
};
