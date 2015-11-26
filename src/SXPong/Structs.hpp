#pragma once

#include "stdafx.h"
#include <vector>

// The screen drawing buffer - holds all buffers used for drawing
struct ScreenDrawingBuffer
{
	HWND hwnd;					// Handle to the window to draw on
	RECT scrnRect;				// Rect holding the window coordinates
	HDC hdcFront;				// The dc the user sees
	HDC hdcBack;				// Used for drawing in memory
	HDC hdcBackground;			// Contains everything in the picture that doesn't change
	HANDLE hbmBack;				// Handle to the bitmap used in hdcBack
	HANDLE hbmBackground;		// Handle to the bitmap used in hdcBackground
	HANDLE hbmOld;				// Handle to the old hbm of hdcBack
	HANDLE hbmOld2;				// Handle to the old hbm of hdcBackground
};

// FrameData - Holds all the needed data to render one separate frame
struct FrameData
{
	double ballX;
	double ballY;
	double playerX;
	double playerY;
	double ballVx;
	double ballVy;
	int playerScore;
	int	opponentScore;
	bool isLeft;
};

struct Packet
{
	int message;
	int uniqueID;
	double ballY;
	double ballVx;
	double ballVy;
};

