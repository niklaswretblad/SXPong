#pragma once

#include "png.h"
#include <d3d9.h>
#include "Constants.hpp"
#include "Structs.hpp"
#include <map>
#include <vector>


#define MAINMENU 1
#define SCORESCREEN 2
#define WINSCREEN 3
#define JOININGSCREEN 4
#define JOINEDSCREEN 5

#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

struct CustomVertex
{
	float x; // coordinate in the x-plane 
	float y; // coordinate in the y plane 
	float z; // coordinate in the z-plane 
	float rhw;
	D3DCOLOR color;
	float tx;
	float ty;
};

struct NormalizedPoint
{
	float x;
	float y;
};

class Renderer
{
public:
	Renderer(HWND);
	~Renderer();

	void createDrawingBuffers(HWND);
	void resizeBuffers(HWND);
	void setDisplayedWinMessage(LPWSTR);

	void renderMainMenu();
	void renderScoreScreen(bool iScored);
	void render_Frame(FrameData fd);
	void renderWinScreen(bool iWon, bool isLeftPlayer);
	void renderConnectingToOpponent();
	void renderConnected();
	void clearDCs();
	void setLeftPlayerStrings();
	void setRightPlayerStrings();
	RECT getScrnRect() { return drawingBuff.scrnRect; }
	void setScoreStr(std::string str) { scoreStr = str; }
	void setOpponentScoreStr(std::string str) { opponentScoreStr = str; }

protected:
	int lastDisplayedWindow;

	void freeDrawingBuffers();
	void createMainMenuButtons();

	void displayBitMap(HBITMAP, double x, double y);

	double getOnePercentOfX();
	double getOnePercentOfY();

	ScreenDrawingBuffer drawingBuff;
	FrameData lastDisplayedFrame;
	HBRUSH BLACKBRUSH, WHITEBRUSH;

	std::wstring startGameText, restartGameText;

	// Direct X
	std::string scoreStr, opponentScoreStr;
	int textureWidth;
	int textureHeight;
	int scoreStrOffset;
	const float letterTextureWidth;
	const float letterTextureHeight;

	void createDevice();
	void releaseDevice();

	void createVertices(FrameData);
	std::vector<CustomVertex> getBoardVertices();
	std::vector<CustomVertex> getStringVertices(std::string, int x, int y, int width, int height);
	void renderString(int vertexOffSet, int length);
	void loadAlphabetTexture();
	void loadAlphabetOffsets();
	void loadWhiteTexture();

	std::map<char, POINT> alphabetOffsets;
	std::map<char, NormalizedPoint> normalizedAlphabetOffsets;
	IDirect3DDevice9* device;
	IDirect3D9* d3d;
	IDirect3DVertexBuffer9* v_buffer;
	IDirect3DTexture9* alphabetTexture;
	IDirect3DTexture9* whiteTexture;
};