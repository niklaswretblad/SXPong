#pragma once

#include "GameObject.hpp"
#include "Ball.hpp"
#include "Player.hpp"

namespace CollisionCheck
{
	bool objectsCollided(GameObject*, GameObject*);
	bool objectCollidedWithWall(GameObject*);
	bool ballPassedEnemyLines(GameObject* ball, GameObject* player);
	bool ballCollidedWithPlayer(Ball* ball, Player* player, RECT scrnRect);
	bool ballCollidedWithWall(Ball* ball, RECT scrnRect);
}

