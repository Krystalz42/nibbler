#pragma once

#include <string>
#include <ostream>

#define MAX_SNAKE 8
#define CHAT_BUFFER 128
#define COMPONENT_MAX 7
#define BASE_ENTITIES_CAPACITY 20

enum eDirection {
	NORTH,
	SOUTH,
	EAST,
	WEST
};

enum eJoystick {
	NONE,
	ARROW_UP,
	ARROW_DOWN,
	ARROW_LEFT,
	ARROW_RIGHT
};


enum eSnakeSprite {
	GREEN,
	PURPLE,
	BLUE,
	VOID
};

struct		Snake
{
	Snake() : name(""), sprite(VOID), isReady(false), id(-1) {};

	std::string		name;
	eSnakeSprite	sprite;
	bool			isReady;
	int16_t				id;

	friend std::ostream &operator<<(std::ostream &os, const Snake &snake) {
		os << "name: " << snake.name << " sprite: " << snake.sprite
		   << " isReady: " << snake.isReady << " id: " << snake.id;
		return os;
	}

	Snake &operator=(Snake  const &snake) {
		if (this != &snake) {
			name = snake.name;
			sprite = snake.sprite;
			isReady = snake.isReady;
			id = snake.id;
		}
		return *this;
	}

	static Snake randomSnake(int16_t id) {
		Snake snake;

		snake.sprite = static_cast<eSnakeSprite>(rand() % VOID);
		snake.name = Snake::basicName[rand() % MAX_SNAKE];
		snake.id = id;
		return (snake);
	}

	static int getlastSnakeIndex(Snake *snakes, unsigned int range) {
		for (unsigned int i = 0; i < range; i++)
		{
			if (snakes[i].id == -1)
				return (i);
		}
		return (-1);
	}

	static int isFull(Snake *snakes, unsigned int range) {
		for (unsigned int i = 0; i < range; i++)
		{
			if (snakes[i].id == -1)
				return (false);
		}
		return (true);
	}

	static int getSnakeById(Snake *snakes, unsigned int range, int16_t id) {
		for (unsigned int i = 0; i < range; i++)
		{
			if (snakes[i].id == id)
				return (i);
		}
		return (-1);
	}

	static std::string const basicName[MAX_SNAKE];
};

//ADD SIZE IN ClientTCP::size_header

enum eHeader {
	CHAT,				//0
	FOOD,				//1
	ID,					//2
	START_GAME,			//3
	SNAKE,				//4
	SNAKE_ARRAY,		//5
	HEADER				//6
};
