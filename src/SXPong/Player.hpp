#pragma once
#include "GameObject.hpp"

enum struct Direction { UP, DOWN, STOPPED };

class Player :
	public GameObject
{
public:
	Player();
	~Player();

	Direction getDirection() { return direction; }
	int getScore() { return score; }
	void resetScore() { score = 0; }

	void setY(double y);
	void setAsLeftPlayer();
	void setAsRightPlayer();
	void setDirection(Direction direction) { this->direction = direction; }
	bool isLeftPlayer() { return (getX() < 50); }

	void move(long deltaTime);
	void incrementScore() { score++; }
protected:
	Direction direction;
	double playerVy;
	int score;
};

