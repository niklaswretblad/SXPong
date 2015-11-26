#include "stdafx.h"
#include "Ball.hpp"
#include "Constants.hpp"
#include <cmath>
#include <random>

Ball::Ball()
{
	setX(static_cast<double>(BALL_STARTING_X));
	setY(static_cast<double>(BALL_STARTING_Y));
	setWidth(static_cast<double>(BALL_WIDTH));
	setHeight(static_cast<double>(BALL_HEIGHT));
	randomizeHeading();
}


Ball::~Ball()
{
}

void Ball::randomizeHeading()
{
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist5_max(5, BALLMAXSPEED - (BALLMAXSPEED / 3));
	std::uniform_int_distribution<std::mt19937::result_type> dist0_1(0, 1);
	
	ballVx = dist5_max(rng);
	ballVx *= -1;

	if (dist0_1(rng))
	{
		ballVy = dist5_max(rng);
		ballVy *= -1;
	}
	else
	{
		ballVy = dist5_max(rng);
	}
}

void Ball::move(long deltaTime)
{
	x += (ballVx * (deltaTime / static_cast<double>(1000)));
	y += (ballVy * (deltaTime / static_cast<double>(1000)));
	if (getX() > 1000)
		std::cout << "> 1000\n";
}

void Ball::bounceAgainstWall()
{
	if (ballVy <= 0)
	{
		if (y < 0 + BALL_R)
			y = 0 + 1;
	}
	else
	{
		if (y + BALL_R + 0.2 > 100)
			y = 100 - BALL_R;
	}

	ballVy *= -1;
}

void Ball::bounceAgainstPlayer(Player* player)
{
	double relativeIntersectY = (player->getY() + (PLAYER_HEIGHT / 2)) - y;
	double normalizedRelativeIntersectionY = (relativeIntersectY / (PLAYER_HEIGHT / 2));
	double bounceAngle = normalizedRelativeIntersectionY * MAXBOUNCEANGLE;
	
	ballVx = BALLMAXSPEED * cos(bounceAngle);
	ballVy = BALLMAXSPEED * -(sin(bounceAngle));

	if (ballVx < 0)
		ballVx -= BALLMINIMUMSPEED;
	else
		ballVy += BALLMINIMUMSPEED;

	if (ballVy < 0)
		ballVy -= BALLMINIMUMSPEED;
	else
		ballVy += BALLMINIMUMSPEED;

	if (player->getX() < 50)
	{
		setX(player->getX() + player->getWidth() + BALL_R + 0.01);
	}
	else
	{
		ballVx *= -1;
		setX(player->getX() - BALL_R - 0.01);
	}
}

void Ball::setVxVy(double vx, double vy)
{
	ballVx = vx;
	ballVy = vy;
}

double Ball::getVx()
{
	return ballVx;
}

double Ball::getVy()
{
	return ballVy;
}
