#ifndef PLAYER_DEFINED
#define PLAYER_DEFINED

#include "Player.hpp"
#include <iostream>

class Ball :
	public GameObject
{
public:
	Ball();
	~Ball();

	void move(long deltaTime);
	void bounceAgainstWall();
	void bounceAgainstPlayer(Player*);
	void randomizeHeading();
	void setVxVy(double Vx, double Vy);
	double getVy();
	double getVx();

protected:
	double ballVx, ballVy;

};

#endif