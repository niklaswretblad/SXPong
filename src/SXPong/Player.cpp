#include "stdafx.h"
#include "Player.hpp"
#include "Constants.hpp"

Player::Player()
{
	setWidth(PLAYER_WIDTH);
	setHeight(PLAYER_HEIGHT);
	setY(PLAYER_STARTING_Y);
	setDirection(Direction::STOPPED);
	playerVy = PLAYERSPEED;
	resetScore();
}


Player::~Player()
{
}

void Player::move(long deltaTime)
{
	switch (direction)
	{
	case Direction::UP:
		if (y > 0)
			setY(getY() - (playerVy * (deltaTime / static_cast<double>(1000))));
		break;
	case Direction::DOWN:
		if (y + height < 100)
		setY(getY() + (playerVy * (deltaTime / static_cast<double>(1000))));
		break;
	}
}

void Player::setY(double y)
{
	if (y > 0 && y < 100)
	{
		this->y = y;
	}
}

void Player::setAsLeftPlayer()
{
	setX(LEFTPLAYER_X);
}

void Player::setAsRightPlayer()
{
	setX(RIGHTPLAYER_X);
}