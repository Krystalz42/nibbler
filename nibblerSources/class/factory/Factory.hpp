#pragma once

#include <nibbler.hpp>
#include <cores/Snake.hpp>

class Univers;

class Factory {
public:
	Factory(Univers &univers);

	void createAllSnake(std::array<Snake, SNAKE_MAX> snake_array, int16_t nu);

private:

	void createSnake(Snake const &snake, int maxSnakes);

	void createWalls();

	void createWall(int x, int y);

	Univers &univers_;
};
