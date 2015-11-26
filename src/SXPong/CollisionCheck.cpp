#include "stdafx.h"
#include "CollisionCheck.hpp"
#include "Constants.hpp"

bool CollisionCheck::objectsCollided(GameObject* object1, GameObject* object2)
{
	if (object1->getX() > object2->getX() + object2->getWidth() ||
		object1->getX() + object1->getWidth() < object2->getX() ||
		object1->getY() > object2->getY() + object2->getHeight() ||
		object1->getY() + object1->getHeight() < object2->getHeight())
	{
		return false;
	}
	return true;
}

bool CollisionCheck::ballCollidedWithPlayer(Ball* ball, Player* player, RECT scrnRect)
{
	float height = BALL_R * (static_cast<float>(scrnRect.right) / scrnRect.bottom);
	float width = BALL_R;

	if (player->isLeftPlayer())
	{
		if (ball->getX() - (width / 2) > player->getX() + player->getWidth() ||
			ball->getX() + (width / 2) < player->getX() - player->getWidth() ||
			ball->getY() - (height / 2) > player->getY() + player->getHeight() ||
			ball->getY() + (height / 2) < player->getY())
			return false;
	}
	else
	{
		if (ball->getX() - (width / 2) > player->getX() + (player->getWidth() * 2) ||
			ball->getX() + (width / 2) < player->getX() ||
			ball->getY() - (height / 2) > player->getY() + player->getHeight() ||
			ball->getY() + (height / 2) < player->getY())
			return false;
	}
	return true;
}

bool CollisionCheck::objectCollidedWithWall(GameObject* gameObject)
{
	if (gameObject->getY() <= 0)
	{
		return true;
	}
	else if (gameObject->getY() + gameObject->getHeight() >= 100)
	{
		return true;
	}
	return false;
}

bool CollisionCheck::ballPassedEnemyLines(GameObject* ball, GameObject* player)
{
	if (player->getX() < 50)
	{
		if (ball->getX() < player->getX() - (player->getWidth()))
		{
			return true;
		}
	}
	else
	{
		if (ball->getX() + BALL_R > player->getX() + player->getWidth())
		{
			return true;
		}
	}
	return false;
}

bool CollisionCheck::ballCollidedWithWall(Ball* ball, RECT scrnRect)
{
	float height = BALL_R * (static_cast<float>(scrnRect.right) / scrnRect.bottom);

	if (ball->getY() + (height / 2) >= 100 ||
		ball->getY() - (height / 2) <= 0)
		return true;
	return false;
}