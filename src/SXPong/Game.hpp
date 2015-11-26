#pragma once

#include "Player.hpp"
#include "Ball.hpp"
#include "Renderer.hpp"
#include "Networker.hpp"

#define FPS 30

enum struct LatestScorer { ME, OPPONENT };
enum struct State { INGAME, INMAINMENU, INWINSCREEN, CONNECTINGTOOPPONENT, SOMEONESCORED, FOUNDGAME };

struct GameState
{
	State state;
	LatestScorer latestScorer;
};

class Game
{
public:
	Game(HWND);
	~Game();

	void run();
	void handleRawKeyPress(UINT);
	void handleRawMouseInput(RAWINPUT* rawInput);
	void handlePushButtonClick(int buttonID);
	void handleMouseMove(int x, int y);

private:
	Player player;
	Ball ball;
	unsigned long lastTime;
	bool hasBall;
	bool opponentRecievedBall;
	int opponentScore;

	GameState gameState;

	Renderer renderer;
	Networker networker;

	void collisionCheck();
	FrameData getCurrentFrameData();
	void fullRestart();
	void reset();
	void setScoreStrings();

	void handleMessage(Packet);
	void sendTakeBallMessage();
	void sendIHaveBallMessage();
	void sendYouScoredMessage();
	void sendYouWonMessage();
};

