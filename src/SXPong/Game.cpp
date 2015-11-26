#include "stdafx.h"
#include "Game.hpp"
#include <chrono>
#include <ctime>
#include "CollisionCheck.hpp"
#include <thread>

Game::Game(HWND hWnd) : opponentScore(0), renderer(hWnd)
{
	lastTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	player.setAsLeftPlayer();
	renderer.setLeftPlayerStrings();
	renderer.createDrawingBuffers(hWnd);
	gameState.state = State::INMAINMENU;
}

Game::~Game()
{
}

void Game::run()
{
	unsigned long timeNow = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	int deltaTime = timeNow - lastTime;
	lastTime = timeNow;

	if (deltaTime < (1000 / FPS))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds((1000 / FPS) - deltaTime));
	}

	switch (gameState.state)
	{
	case State::INGAME:
	{
		handleMessage(networker.recvPacket());
		if (hasBall)
		{
			ball.move(deltaTime);
			collisionCheck();
			sendIHaveBallMessage();
		}
		else
		{
			if (!opponentRecievedBall)
			{
				sendTakeBallMessage();
			}
		}

		player.move(deltaTime);

		FrameData fd = getCurrentFrameData();
		if (!hasBall && fd.ballX != 110) fd.ballX = 110;
		renderer.render_Frame(fd);
		break;
	}
	case State::CONNECTINGTOOPPONENT:
		if (networker.connect(player.isLeftPlayer()))
		{
			gameState.state = State::FOUNDGAME;
		}
		renderer.renderConnectingToOpponent();
		break;
	case State::FOUNDGAME:
		renderer.renderConnected();
		handleMessage(networker.recvPacket());
		if (!player.isLeftPlayer())
		{
			Packet packet = { 0 };
			packet.message = WAITINGFORSTARTMSG;
			networker.sendMsg(packet);
		}
		break;
	case State::INMAINMENU:
		renderer.renderMainMenu();
		break;
	case State::INWINSCREEN:
		handleMessage(networker.recvPacket());
		if (gameState.latestScorer == LatestScorer::ME)
			renderer.renderWinScreen(true, player.isLeftPlayer());
		else
		{
			renderer.renderWinScreen(false, player.isLeftPlayer());
			sendYouWonMessage();
		}
		break;
	case State::SOMEONESCORED:
		handleMessage(networker.recvPacket());
		if (gameState.latestScorer == LatestScorer::ME)
			renderer.renderScoreScreen(true);
		else
		{
			renderer.renderScoreScreen(false);
			sendYouScoredMessage();
		}
		break;
	default:
		break;
	}
}

void Game::sendTakeBallMessage()
{
	Packet packet = {};
	packet.message = YOUTAKEBALLMSG;
	packet.ballY = ball.getY();
	packet.ballVx = ball.getVx();
	packet.ballVy = ball.getVy();
	networker.sendMsg(packet);
	printf("sending takeballmsg\n");
}

void Game::sendIHaveBallMessage()
{
	Packet packet = {};
	packet.message = IHAVEBALLMSG;
	networker.sendMsg(packet);
	printf("sending ihaveballmsg\n");
}

void Game::sendYouScoredMessage()
{
	Packet packet = {};
	packet.message = YOUSCOREDMSG;
	networker.sendMsg(packet);
	printf("sending youscoredmsg\n");
}

void Game::sendYouWonMessage()
{
	Packet packet = {};
	packet.message = YOUWONMSG;
	networker.sendMsg(packet);
	printf("sending youwonmsg\n");
}

void Game::handleMessage(Packet packet)
{
	switch (packet.message)
	{
	case YOUTAKEBALLMSG:
		if (!hasBall)
		{
			opponentRecievedBall = false;
			hasBall = true;
			ball.setY(packet.ballY);
			ball.setVxVy(packet.ballVx, packet.ballVy);
			if (player.isLeftPlayer()) 
				ball.setX(100);
			else 
				ball.setX(0);
		}
		break;
	case IHAVEBALLMSG:
		switch (gameState.state)
		{
		case State::INGAME:
			opponentRecievedBall = true;
			break;
		case State::FOUNDGAME:
			gameState.state = State::INGAME;
			break;
		case State::INWINSCREEN:
			fullRestart();
			gameState.state = State::INGAME;
			break;
		case State::SOMEONESCORED:
			gameState.state = State::INGAME;
			reset();
			break;
		default:
			break;
		}
		break;
	case YOUSCOREDMSG:
		if (gameState.state != State::SOMEONESCORED && gameState.state != State::INWINSCREEN && (gameState.state == State::INGAME && !hasBall))
		{
			player.incrementScore();
			setScoreStrings();
			gameState.latestScorer = LatestScorer::ME;
			if (player.getScore() >= ROUNDS_TO_WIN)
				gameState.state = State::INWINSCREEN;
			else
				gameState.state = State::SOMEONESCORED;
			std::cout << std::to_string(opponentScore) << std::endl;
		}
		break;
	case YOUWONMSG:
		if (gameState.state == State::INGAME && player.getScore() > 0)
		{
			gameState.state = State::INWINSCREEN;
			gameState.latestScorer = LatestScorer::ME;
		}
		break;
	default:
		break;
	}
}

void Game::fullRestart()
{
	if (player.isLeftPlayer())
	{
		ball.setX(100);
		ball.randomizeHeading();
		hasBall = true;
		player.resetScore();
		opponentRecievedBall = false;
	}
	else
	{
		ball.setX(110);
		hasBall = false;
		opponentRecievedBall = true;
		player.resetScore();
	}
	opponentScore = 0;
	setScoreStrings();
}

void Game::reset()
{
	if (player.isLeftPlayer())
	{
		ball.setX(100);
		ball.randomizeHeading();
		hasBall = true;
		opponentRecievedBall = false;
	}
	else
	{
		ball.setX(110);
		hasBall = false;
		opponentRecievedBall = true;
	}

	setScoreStrings();
}

void Game::setScoreStrings()
{
	renderer.setScoreStr("score" + std::to_string(player.getScore()));
	renderer.setOpponentScoreStr("opponentscore" + std::to_string(opponentScore));
}

void Game::collisionCheck()
{
	if (hasBall)
	{
		RECT scrnRect = renderer.getScrnRect();

		if (CollisionCheck::ballCollidedWithWall(&ball, scrnRect))
		{
			ball.bounceAgainstWall();
		}

		if (CollisionCheck::ballCollidedWithPlayer(&ball, &player, scrnRect))
		{
			ball.bounceAgainstPlayer(&player);
		}

		if (CollisionCheck::ballPassedEnemyLines(&ball, &player))
		{
			opponentScore++;
			gameState.latestScorer = LatestScorer::OPPONENT;
			setScoreStrings();

			if (opponentScore >= ROUNDS_TO_WIN)
			{
				gameState.state = State::INWINSCREEN;
			}
			else
			{
				gameState.state = State::SOMEONESCORED;
			}
		}

		if ((player.isLeftPlayer() && ball.getX() >= 100) ||
			(!player.isLeftPlayer() && ball.getX() <= 0))
		{
			hasBall = false;
			std::cout << "hasBall=FALSE\n";
		}
	}
}

FrameData Game::getCurrentFrameData()
{
	FrameData fd = { 0 };
	fd.ballX = ball.getX();
	fd.ballY = ball.getY();
	fd.playerX = player.getX();
	fd.playerY = player.getY();
	fd.playerScore = player.getScore();
	fd.opponentScore = opponentScore;
	fd.isLeft = player.isLeftPlayer();
	return fd;
}

void Game::handleRawKeyPress(UINT keyCode)
{
	switch (keyCode)
	{
	case VK_ESCAPE:
		gameState.state = State::INMAINMENU;
		break;
	case VK_UP:
		if (gameState.state == State::INGAME)
		{
			
		}
		break;
	case VK_DOWN:
		if (gameState.state == State::INGAME)
		{
			
		}
		break;
	default:
		break;
	}
}

void Game::handleRawMouseInput(RAWINPUT* rawInput)
{
	bool buttonState;
	if (buttonState = rawInput->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN)
	{
		switch (gameState.state)
		{
		case State::INGAME:
			player.setDirection(Direction::UP);
			break;
		case State::INMAINMENU:
			break;
		case State::INWINSCREEN:
			break;
		case State::CONNECTINGTOOPPONENT:
			break;
		case State::FOUNDGAME:
			break;
		case State::SOMEONESCORED:
			break;
		default:
			break;
		}
	}
	else if (buttonState = rawInput->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN)
	{
		switch (gameState.state)
		{
		case State::INGAME:
			player.setDirection(Direction::DOWN);
			break;
		case State::INMAINMENU:
			break;
		case State::INWINSCREEN:
			break;
		case State::CONNECTINGTOOPPONENT:
			gameState.state = State::INMAINMENU;
			networker.reset();
			break;
		case State::FOUNDGAME:
			break;
			break;
		case State::SOMEONESCORED:
			break;
		default:
			break;
		}
	}
	else if (buttonState = rawInput->data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN)
	{
		switch (gameState.state)
		{
		case State::INGAME:
			break;
		case State::INMAINMENU:
			break;
		case State::INWINSCREEN:
			break;
		case State::CONNECTINGTOOPPONENT:
			break;
		case State::FOUNDGAME:
			if (player.isLeftPlayer())
			{
				gameState.state = State::INGAME;

			}
			break;
		case State::SOMEONESCORED:
			if (player.isLeftPlayer())
			{
				gameState.state = State::INGAME;
				reset();
			}
			break;
		default:
			break;
		}
	}
	else if (buttonState = rawInput->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_UP)
	{
		switch (gameState.state)
		{
		case State::INGAME:
			player.setDirection(Direction::STOPPED);
			break;
		case State::INMAINMENU:
			break;
		case State::INWINSCREEN:
			break;
		case State::CONNECTINGTOOPPONENT:
			break;
		case State::SOMEONESCORED:
			break;
		default:
			break;
		}
	}
	else if (buttonState = rawInput->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_UP)
	{
		switch (gameState.state)
		{
		case State::INGAME:
			player.setDirection(Direction::STOPPED);
			break;
		case State::INMAINMENU:
			break;
		case State::INWINSCREEN:
			break;
		case State::CONNECTINGTOOPPONENT:
			break;
		case State::SOMEONESCORED:
			break;
		default:
			break;
		}
	}
}

void Game::handlePushButtonClick(int buttonID)
{
	switch (buttonID)
	{
	case LEFTPLAYER_BUTTON_ID:
		player.setAsLeftPlayer();
		renderer.setLeftPlayerStrings();
		gameState.state = State::CONNECTINGTOOPPONENT;
		hasBall = true;
		opponentRecievedBall = false;
		ball.setX(100);
		break;
	case RIGHTPLAYER_BUTTON_ID:
		player.setAsRightPlayer();
		renderer.setRightPlayerStrings();
		gameState.state = State::CONNECTINGTOOPPONENT;
		hasBall = false;
		opponentRecievedBall = true;
		break;
	case MAINMENU_BUTTON_ID:
		fullRestart();
		networker.reset();
		gameState.state = State::INMAINMENU;
		break;
	case RESTART_BUTTON_ID:
		if (player.isLeftPlayer())
		{
			fullRestart();
			gameState.state = State::INGAME;
		}
		break;
	default:
		break;
	}
}