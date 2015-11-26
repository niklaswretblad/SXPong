#include "stdafx.h"
#include "Renderer.hpp"

Renderer::Renderer(HWND hWnd) :
BLACKBRUSH(reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH))),
WHITEBRUSH(reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH))),
letterTextureHeight(32), letterTextureWidth(32)
{
	createDrawingBuffers(hWnd);
	createDevice();
	loadWhiteTexture();
	loadAlphabetTexture();
	loadAlphabetOffsets();
	scoreStr = "score";
	opponentScoreStr = "opponentscore";
}

Renderer::~Renderer()
{
	releaseDevice();
	freeDrawingBuffers();
}

void Renderer::renderMainMenu()
{
	if (lastDisplayedWindow != MAINMENU)
	{
		FillRect(drawingBuff.hdcBack, &drawingBuff.scrnRect, BLACKBRUSH);

		HBITMAP seacrossBmp = (HBITMAP)LoadImage(NULL, L"seacross_icon.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE | LR_VGACOLOR);
		displayBitMap(seacrossBmp, 22, 28);

		double nHeight = ((static_cast<double>(drawingBuff.scrnRect.right) / 1000) * (static_cast<double>(drawingBuff.scrnRect.bottom) / 1000)) * 110;

		HFONT hFont = CreateFont(nHeight, 0, 0, 0, FW_BOLD, TRUE, FALSE, FALSE, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, 0, 0, L"Arial");

		HFONT hOldFont = (HFONT)SelectObject(drawingBuff.hdcBack, hFont);

		SetBkMode(drawingBuff.hdcBack, TRANSPARENT);
		SetTextColor(drawingBuff.hdcBack, RGB(255, 255, 255));

		RECT rect = {};
		SetRect(&rect,
			37 * getOnePercentOfX(),
			30 * getOnePercentOfY(),
			90 * getOnePercentOfX(),
			60 * getOnePercentOfY());

		// Draw Pong Title
		std::wstring title = L"SXPong";
		DrawText(drawingBuff.hdcBack,
			title.c_str(),
			title.length(), &rect, DT_LEFT | DT_VCENTER);

		// Change font for instructions
		nHeight = ((static_cast<double>(drawingBuff.scrnRect.right) / 1000) * (static_cast<double>(drawingBuff.scrnRect.bottom) / 1000)) * 17;
		hFont = CreateFont(nHeight, 0, 0, 0, 0, FALSE, FALSE, FALSE, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, 0, 0, L"Arial Black");
		SelectObject(drawingBuff.hdcBack, hFont);

		rect = {};
		//Draw instructions
		SetRect(&rect,
			32 * getOnePercentOfX(),
			58 * getOnePercentOfY(),
			65 * getOnePercentOfX(),
			75 * getOnePercentOfY());

		std::wstring instructions = L"PLAY USING LEFT/RIGHT CLICK";

		DrawText(drawingBuff.hdcBack,
			instructions.c_str(),
			instructions.length(), &rect, DT_CENTER | DT_VCENTER);

		SetRect(&rect,
			32 * getOnePercentOfX(),
			70 * getOnePercentOfY(),
			65 * getOnePercentOfX(),
			80 * getOnePercentOfY());

		std::wstring playAsStr = L"PLAY AS...";

		DrawText(drawingBuff.hdcBack,
			playAsStr.c_str(),
			playAsStr.length(), &rect, DT_CENTER | DT_VCENTER);

		createMainMenuButtons();

		SelectObject(drawingBuff.hdcBack, hOldFont);
		DeleteObject(hFont);

		BitBlt(drawingBuff.hdcFront, 0, 0,
			drawingBuff.scrnRect.right,
			drawingBuff.scrnRect.bottom,
			drawingBuff.hdcBack, 0, 0, SRCCOPY);

		lastDisplayedWindow = MAINMENU;
	}
}

void Renderer::renderConnectingToOpponent()
{
	if (lastDisplayedWindow != JOININGSCREEN)
	{
		FillRect(drawingBuff.hdcBack, &drawingBuff.scrnRect, BLACKBRUSH);

		// Fontsettings 
		HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		LOGFONT logfont;
		GetObject(hFont, sizeof(LOGFONT), &logfont);

		logfont.lfHeight = ((static_cast<double>(drawingBuff.scrnRect.right) / 1000) * (static_cast<double>(drawingBuff.scrnRect.bottom) / 1000)) * 35;         // Fontsize

		HFONT hNewFont = CreateFontIndirect(&logfont);
		HFONT hOldFont = (HFONT)SelectObject(drawingBuff.hdcBack, hNewFont);

		// Textsettings
		SetBkMode(drawingBuff.hdcBack, TRANSPARENT);
		SetTextColor(drawingBuff.hdcBack, RGB(255, 255, 255));

		// Draw PONG-title
		RECT rect = {};
		SetRect(&rect,
			20 * getOnePercentOfX(),
			33 * getOnePercentOfY(),
			80 * getOnePercentOfX(),
			50 * getOnePercentOfY());

		std::wstring searchingForOpponentStr = L"Searching for opponent...";
		
		DrawText(drawingBuff.hdcBack,
			searchingForOpponentStr.c_str(),
			searchingForOpponentStr.length(),
			&rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		SelectObject(drawingBuff.hdcBack, hOldFont);
		DeleteObject(hNewFont);

		BitBlt(drawingBuff.hdcFront, 0, 0,
			drawingBuff.scrnRect.right,
			drawingBuff.scrnRect.bottom,
			drawingBuff.hdcBack, 0, 0, SRCCOPY);

		lastDisplayedWindow = JOININGSCREEN;
	}
}

void Renderer::renderConnected()
{
	if (lastDisplayedWindow != JOINEDSCREEN)
	{
		FillRect(drawingBuff.hdcBack, &drawingBuff.scrnRect, BLACKBRUSH);

		// Fontsettings 

		double lfHeight = ((static_cast<double>(drawingBuff.scrnRect.right) / 1000) * (static_cast<double>(drawingBuff.scrnRect.bottom) / 1000)) * 30;         // Fontsize

		HFONT hNewFont = CreateFont(lfHeight, 0, 0, 0, 0,
			FALSE, FALSE, FALSE, OEM_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS,
			0, 0, L"Arial Black");
		HFONT hOldFont = (HFONT)SelectObject(drawingBuff.hdcBack, hNewFont);

		// Textsettings
		SetBkMode(drawingBuff.hdcBack, TRANSPARENT);
		SetTextColor(drawingBuff.hdcBack, RGB(255, 255, 255));

		RECT rect = {};
		SetRect(&rect,
			20 * getOnePercentOfX(),
			33 * getOnePercentOfY(),
			80 * getOnePercentOfX(),
			50 * getOnePercentOfY());

		std::wstring foundOpponentText = L"Found opponent!";

		DrawText(drawingBuff.hdcBack,
			foundOpponentText.c_str(),
			foundOpponentText.length(),
			&rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		double nHeight = ((static_cast<double>(drawingBuff.scrnRect.right) / 1000) * (static_cast<double>(drawingBuff.scrnRect.bottom) / 1000)) * 25;
		HFONT startGameFont = CreateFont(nHeight, 0, 0, 0, 0,
			FALSE, FALSE, FALSE, OEM_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS,
			0, 0, L"Arial Black");
		SelectObject(drawingBuff.hdcBack, startGameFont);

		rect.top = 55 * getOnePercentOfY();
		rect.bottom = 70 * getOnePercentOfY();

		DrawText(drawingBuff.hdcBack,
			startGameText.c_str(),
			startGameText.length(),
			&rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		BitBlt(drawingBuff.hdcFront, 0, 0, drawingBuff.scrnRect.right, 
			drawingBuff.scrnRect.bottom, drawingBuff.hdcBack, 0, 0, SRCCOPY);

		SelectObject(drawingBuff.hdcBack, hOldFont);
		DeleteObject(hNewFont);
		DeleteObject(startGameFont);
	}
}

void Renderer::renderScoreScreen(bool iScored)
{
	if (lastDisplayedWindow != SCORESCREEN)
	{
		FillRect(drawingBuff.hdcBack, &drawingBuff.scrnRect, BLACKBRUSH);

		double nHeight = ((static_cast<double>(drawingBuff.scrnRect.right) / 1000) * (static_cast<double>(drawingBuff.scrnRect.bottom) / 1000)) * 30;

		HFONT scoreFont = CreateFont(nHeight, 0, 0, 0, 0,
			FALSE, FALSE, FALSE, OEM_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS,
			0, 0, L"Arial Black");

		HANDLE oldFont = SelectObject(drawingBuff.hdcBack, scoreFont);

		std::wstring scoreStr;

		if (iScored)
		{
			scoreStr = L"You scored! " + startGameText;
		}
		else
		{
			scoreStr = L"Opponent scored! " + startGameText;
		}

		RECT rect = {};
		rect.left = 10 * getOnePercentOfX();
		rect.right = rect.left + (80 * getOnePercentOfX());
		rect.top = 45 * getOnePercentOfY();
		rect.bottom = 60 * getOnePercentOfY();

		DrawText(drawingBuff.hdcBack, scoreStr.c_str(),
			scoreStr.length(), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		BitBlt(drawingBuff.hdcFront, 0, 0,
			drawingBuff.scrnRect.right,
			drawingBuff.scrnRect.bottom,
			drawingBuff.hdcBack, 0, 0, SRCCOPY);

		SelectObject(drawingBuff.hdcBack, oldFont);
		DeleteObject(scoreFont);
		lastDisplayedWindow = SCORESCREEN;
	}
}

void Renderer::renderWinScreen(bool iWon, bool isLeftPlayer)
{
	if (lastDisplayedWindow != WINSCREEN)
	{
		FillRect(drawingBuff.hdcBack, &drawingBuff.scrnRect, BLACKBRUSH);

		std::wstring scoreMessageStr;

		if (iWon)
		{
			scoreMessageStr = L"YOU WON!!!";
		}
		else
		{
			scoreMessageStr = L"YOU LOST!!!";
		}

		RECT rect = {};
		SetRect(&rect,
			33 * getOnePercentOfX(),
			33 * getOnePercentOfY(),
			66 * getOnePercentOfX(),
			50 * getOnePercentOfY());

		double nHeight = ((static_cast<double>(drawingBuff.scrnRect.right) / 1000) * (static_cast<double>(drawingBuff.scrnRect.bottom) / 1000)) * 40;

		HFONT hFont = CreateFont(nHeight, 0, 0, 0, 0, 
			FALSE, FALSE, FALSE, OEM_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, 
			0, 0, L"Arial Black");
		HANDLE oldFont = SelectObject(drawingBuff.hdcBack, hFont);

		// Textsettings
		SetBkMode(drawingBuff.hdcBack, TRANSPARENT);
		SetTextColor(drawingBuff.hdcBack, RGB(255, 255, 255));

		DrawText(drawingBuff.hdcBack,
			scoreMessageStr.c_str(),
			scoreMessageStr.length(),
			&rect, (DT_CENTER | DT_VCENTER | DT_SINGLELINE));

		RECT mainMenuButtonRect = {};
		
		if (isLeftPlayer)
		{
			mainMenuButtonRect.left = getOnePercentOfX() * 20;
			
		}
		else
		{
			mainMenuButtonRect.left = getOnePercentOfX() * 41;
		}

		mainMenuButtonRect.top = getOnePercentOfY() * 80;
		mainMenuButtonRect.right = mainMenuButtonRect.left + (getOnePercentOfX() *  /*Width of button*/ 18);
		mainMenuButtonRect.bottom = mainMenuButtonRect.top + (getOnePercentOfY() * /*Height of button*/ 9);

		CreateWindowEx(NULL,
			L"BUTTON",
			L"MAIN MENU",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			mainMenuButtonRect.left,
			mainMenuButtonRect.top,
			mainMenuButtonRect.right - mainMenuButtonRect.left,
			mainMenuButtonRect.bottom - mainMenuButtonRect.top,
			drawingBuff.hwnd,
			(HMENU)MAINMENU_BUTTON_ID,
			(HINSTANCE)GetWindowLong(drawingBuff.hwnd, GWL_HINSTANCE),
			NULL);

		if (isLeftPlayer)
		{
			CreateWindowEx(NULL,
				L"BUTTON",
				L"RESTART GAME",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				getOnePercentOfX() * 62,
				getOnePercentOfY() * 80,
				getOnePercentOfX() * 18,
				getOnePercentOfY() * 9,
				drawingBuff.hwnd,
				(HMENU)RESTART_BUTTON_ID,
				(HINSTANCE)GetWindowLong(drawingBuff.hwnd, GWL_HINSTANCE),
				NULL);
		}

		BitBlt(drawingBuff.hdcFront, 0, 0,
			drawingBuff.scrnRect.right,
			drawingBuff.scrnRect.bottom,
			drawingBuff.hdcBack, 0, 0, SRCCOPY);

		SelectObject(drawingBuff.hdcBack, oldFont);
		DeleteObject(hFont);

		lastDisplayedWindow = WINSCREEN;
	}
}

void Renderer::setLeftPlayerStrings()
{
	startGameText = L"Press the middle mouse button to start game...";
	restartGameText = L"";
}

void Renderer::setRightPlayerStrings()
{
	startGameText = L"Waiting for leftplayer to start game...";
	restartGameText = L"Waiting for leftplayer to restart game...";
}

void Renderer::createMainMenuButtons()
{
	HWND joinButton, hostButton;

	joinButton = CreateWindowEx(NULL,
		L"BUTTON",
		L"LEFT PLAYER",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		getOnePercentOfX() * 20,
		getOnePercentOfY() * 80,
		getOnePercentOfX() * 18,
		getOnePercentOfY() * 9,
		drawingBuff.hwnd,
		(HMENU)LEFTPLAYER_BUTTON_ID,
		(HINSTANCE)GetWindowLong(drawingBuff.hwnd, GWL_HINSTANCE),
		NULL);

	hostButton = CreateWindowEx(NULL,
		L"BUTTON",
		L"RIGHT PLAYER",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		getOnePercentOfX() * 62,
		getOnePercentOfY() * 80,
		getOnePercentOfX() * 18,
		getOnePercentOfY() * 9,
		drawingBuff.hwnd,
		(HMENU)RIGHTPLAYER_BUTTON_ID,
		(HINSTANCE)GetWindowLong(drawingBuff.hwnd, GWL_HINSTANCE),
		NULL);
}

void Renderer::displayBitMap(HBITMAP hBitMap, double x, double y)
{
	HDC hdcBitmap = CreateCompatibleDC(drawingBuff.hdcFront);
	HANDLE oldBmp = (HBITMAP)SelectObject(hdcBitmap, hBitMap);

	BITMAP bm = { 0 };
	GetObject(hBitMap, sizeof(bm), &bm);

	StretchBlt(drawingBuff.hdcBack, x * getOnePercentOfX(), y * getOnePercentOfY(),
		SEACROSS_ICON_WIDTH * getOnePercentOfX(),
		SEACROSS_ICON_HEIGHT * getOnePercentOfY(),
		hdcBitmap, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
}

double Renderer::getOnePercentOfX()
{
	return static_cast<double>(drawingBuff.scrnRect.right) / 100;
}

double Renderer::getOnePercentOfY()
{
	return static_cast<double>(drawingBuff.scrnRect.bottom) / 100;
}

void Renderer::resizeBuffers(HWND hWnd)
{
	drawingBuff.hwnd = hWnd;
	GetClientRect(hWnd, &drawingBuff.scrnRect);

	SelectObject(drawingBuff.hdcBack, drawingBuff.hbmOld);
	DeleteObject(drawingBuff.hbmBack);

	SelectObject(drawingBuff.hdcBackground, drawingBuff.hbmOld);
	DeleteObject(drawingBuff.hbmBackground);

	drawingBuff.hbmBack = CreateCompatibleBitmap(drawingBuff.hdcBack, drawingBuff.scrnRect.right, drawingBuff.scrnRect.bottom);
	drawingBuff.hbmOld = SelectObject(drawingBuff.hdcBack, drawingBuff.hbmBack);

	drawingBuff.hbmBackground = CreateCompatibleBitmap(drawingBuff.hdcBackground, drawingBuff.scrnRect.right, drawingBuff.scrnRect.bottom);
	drawingBuff.hbmOld2 = SelectObject(drawingBuff.hdcBackground, drawingBuff.hbmBackground);
}

void Renderer::createDrawingBuffers(HWND hWnd)
{
	drawingBuff.hwnd = hWnd;
	drawingBuff.hdcFront = GetDC(hWnd);
	GetClientRect(hWnd, &drawingBuff.scrnRect);

	drawingBuff.hdcBack = CreateCompatibleDC(drawingBuff.hdcFront);
	drawingBuff.hdcBackground = CreateCompatibleDC(drawingBuff.hdcFront);

	drawingBuff.hbmBack = CreateCompatibleBitmap(drawingBuff.hdcFront, drawingBuff.scrnRect.right, drawingBuff.scrnRect.bottom);
	drawingBuff.hbmBackground = CreateCompatibleBitmap(drawingBuff.hdcFront, drawingBuff.scrnRect.right, drawingBuff.scrnRect.bottom);

	drawingBuff.hbmOld = SelectObject(drawingBuff.hdcBack, drawingBuff.hbmBack);
	drawingBuff.hbmOld2 = SelectObject(drawingBuff.hdcBackground, drawingBuff.hbmBackground);

	SetMapMode(drawingBuff.hdcBack, MM_TEXT);
	SetMapMode(drawingBuff.hdcBackground, MM_TEXT);
	SetMapMode(drawingBuff.hdcFront, MM_TEXT);
}

void Renderer::freeDrawingBuffers()
{
	SelectObject(drawingBuff.hdcBack, drawingBuff.hbmOld);
	SelectObject(drawingBuff.hdcBackground, drawingBuff.hbmOld2);

	DeleteObject(drawingBuff.hbmBack);
	DeleteObject(drawingBuff.hbmBackground);

	DeleteDC(drawingBuff.hdcBack);
	DeleteDC(drawingBuff.hdcBackground);

	ReleaseDC(drawingBuff.hwnd, drawingBuff.hdcFront);
}

// DirectX drawing

void Renderer::render_Frame(FrameData fd)
{
	device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	device->BeginScene();    // begins the 3D scene

	createVertices(fd);

	device->SetFVF(CUSTOMFVF);

	device->SetStreamSource(0, v_buffer, 0, sizeof(CustomVertex));

	device->SetTexture(0, alphabetTexture);
	renderString(scoreStrOffset, scoreStr.size() + opponentScoreStr.size());

	device->SetTexture(0, whiteTexture);
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	device->DrawPrimitive(D3DPT_TRIANGLEFAN, 6, 92);

	device->EndScene();    // ends the 3D scene

	device->Present(nullptr, nullptr, nullptr, nullptr);    // displays the created frame

	lastDisplayedWindow = 0; 
}

void Renderer::renderString(int startingOffSet, int length)
{
	for (int i = 0; i < length; ++i)
	{
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, scoreStrOffset + (i * 4), 2);
	}
}

std::vector<CustomVertex> Renderer::getStringVertices(std::string str, int x, int y, int width, int height)
{
	int numOfLetters = str.length();
	std::vector<CustomVertex> vertices;

	for (int i = 0; i < str.length(); ++i)
	{
		if (alphabetOffsets.find(str[i]) != alphabetOffsets.end())
		{
			NormalizedPoint textureOffset = normalizedAlphabetOffsets[str[i]];

			vertices.push_back(
			{ x + ((width / numOfLetters) * i),
			y,
			0.0f,
			1.0f,
			D3DCOLOR_XRGB(255, 255, 255),
			textureOffset.x,
			textureOffset.y });

			vertices.push_back(
			{ x + ((width / numOfLetters) * (i + 1)),
			y,
			0.0f,
			1.0f,
			D3DCOLOR_XRGB(255, 255, 255),
			textureOffset.x + (static_cast<float>(letterTextureWidth) / textureWidth),
			textureOffset.y });

			vertices.push_back(
			{ x + ((width / numOfLetters) * i),
			y + height,
			0.0f,
			1.0f,
			D3DCOLOR_XRGB(255, 255, 255),
			textureOffset.x,
			textureOffset.y + (static_cast<float>(letterTextureHeight) / textureHeight) });

			vertices.push_back(
			{ x + ((width / numOfLetters) * (i + 1)),
			y + height,
			0.0f,
			1.0f,
			D3DCOLOR_XRGB(255, 255, 255),
			textureOffset.x + (static_cast<float>(letterTextureWidth) / textureWidth),
			textureOffset.y + (static_cast<float>(letterTextureHeight) / textureHeight) });
		}
	}
	return vertices;
}

void Renderer::createVertices(FrameData fd)
{
	std::vector<CustomVertex> scoreVertices = getStringVertices(scoreStr,
		5 * getOnePercentOfX(),
		(BOARD_TOP + 2) * getOnePercentOfY(),
		10 * getOnePercentOfX(),
		5 * getOnePercentOfY());

	std::vector<CustomVertex> opponentScoreVertices = getStringVertices(opponentScoreStr,
		73 * getOnePercentOfX(),
		(BOARD_TOP + 2) * getOnePercentOfY(),
		25 * getOnePercentOfX(),
		5 * getOnePercentOfY());

	std::vector<CustomVertex> boardVertices = getBoardVertices();

	std::vector<CustomVertex> playerVertices
	{
		{ fd.playerX * getOnePercentOfX(), fd.playerY * getOnePercentOfY(), 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0.0f },
		{ (fd.playerX + PLAYER_WIDTH) * getOnePercentOfX(), fd.playerY * getOnePercentOfY(), 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0.0f },
		{ (fd.playerX + PLAYER_WIDTH) * getOnePercentOfX(), (fd.playerY + PLAYER_HEIGHT) * getOnePercentOfY(), 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0.0f },
		{ fd.playerX * getOnePercentOfX(), fd.playerY * getOnePercentOfY(), 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0.0f },
		{ (fd.playerX + PLAYER_WIDTH) * getOnePercentOfX(), (fd.playerY + PLAYER_HEIGHT) * getOnePercentOfY(), 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0.0f },
		{ fd.playerX * getOnePercentOfX(), (fd.playerY + PLAYER_HEIGHT) * getOnePercentOfY(), 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0.0f }
	};

	float heightInPix = (BALL_R * (static_cast<float>(drawingBuff.scrnRect.right) / drawingBuff.scrnRect.bottom)) * getOnePercentOfY();
	float widthInPix = (BALL_R * getOnePercentOfX());

	const int numOfVertices = 94;
	std::vector<CustomVertex> ballVertices;
	ballVertices.push_back({ fd.ballX * getOnePercentOfX(), fd.ballY * getOnePercentOfY(), 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0.0f });

	for (int i = (376 / NUMOFBALLVERTICES); i < 376; i += (376 / NUMOFBALLVERTICES))
	{
		float angleInRad = i * (PI / static_cast<float>(180));

		ballVertices.push_back({
			(fd.ballX * getOnePercentOfX()) + (cos(angleInRad) * widthInPix),
			(fd.ballY * getOnePercentOfY()) + (sin(angleInRad) * heightInPix),
			1.0f,
			0.0f,
			D3DCOLOR_XRGB(255, 255, 255),
			0.0f, 0.0f
		});
	}

	std::vector<CustomVertex> vertices;
	//vertices.insert(vertices.end(), boardVertices.begin(), boardVertices.end());
	vertices.insert(vertices.end(), playerVertices.begin(), playerVertices.end());
	vertices.insert(vertices.end(), ballVertices.begin(), ballVertices.end());
	vertices.insert(vertices.end(), scoreVertices.begin(), scoreVertices.end());
	vertices.insert(vertices.end(), opponentScoreVertices.begin(), opponentScoreVertices.end());

	scoreStrOffset = playerVertices.size() + ballVertices.size();

	void* pVoid;

	v_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices.data(), vertices.size() * sizeof(CustomVertex));
	v_buffer->Unlock();
}

std::vector<CustomVertex> Renderer::getBoardVertices()
{
	std::vector<CustomVertex> boardVertices;
	// Upper rect
	boardVertices.push_back({ 0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255) });
	boardVertices.push_back({ drawingBuff.scrnRect.right, 0.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255) });
	boardVertices.push_back({ 0.0f, BOARD_TOP * getOnePercentOfY(), 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255) });
	boardVertices.push_back({ drawingBuff.scrnRect.right, BOARD_TOP * getOnePercentOfY(), 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255) });
	// Lower rect
	boardVertices.push_back({ 0.0f, BOARD_BOTTOM * getOnePercentOfY(), 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255) });
	boardVertices.push_back({ drawingBuff.scrnRect.right, BOARD_BOTTOM * getOnePercentOfY(), 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255) });
	boardVertices.push_back({ 0.0f, drawingBuff.scrnRect.bottom, 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255) });
	boardVertices.push_back({ drawingBuff.scrnRect.right, drawingBuff.scrnRect.bottom, 0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255) });

	return boardVertices;
}

void Renderer::loadWhiteTexture()
{
	int width = 5;
	int height = 5;
	device->CreateTexture(width, height, 0, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, (IDirect3DTexture9**)&whiteTexture, nullptr);

	D3DLOCKED_RECT lockedRect = {};
	whiteTexture->LockRect(0, &lockedRect, nullptr, D3DLOCK_DISCARD);

	memset(lockedRect.pBits, D3DCOLOR_XRGB(255, 255, 255), width * height * 4);

	whiteTexture->UnlockRect(0);
}

void Renderer::loadAlphabetTexture()
{
	char header[8];

	png_byte color_type;
	png_byte bit_depth;
	int width, height;
	int x, y;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	std::vector<png_byte> imageBuffer;
	std::vector<png_bytep> row_pointers;

	FILE* fp = fopen("alphabet.png", "rb");
	if (!fp)
	{
		printf("Failed to load png\n");
		return;
	}

	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_const_bytep)header, 0, 8))
	{
		printf("File is not recognized as a png file\n");
		return;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png_ptr)
	{
		printf("png_create_read_struct failed\n");
		return;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		printf("png_create_info_struct failed\n");
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr,
			nullptr);
		fclose(fp);
		printf("Error during init_io\n");
		return;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	textureWidth = width;
	height = png_get_image_height(png_ptr, info_ptr);
	textureHeight = height;
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("Error during read image\n");
		return;
	}

	int rowBytes = png_get_rowbytes(png_ptr, info_ptr);
	int totalBytes = height * rowBytes;
	imageBuffer.resize(totalBytes);
	row_pointers.resize(height);

	for (int i = 0; i < height; i++)
	{
		row_pointers[i] = imageBuffer.data() + (i * rowBytes);
	}

	png_set_rows(png_ptr, info_ptr, row_pointers.data());
	png_read_image(png_ptr, row_pointers.data());
	png_read_end(png_ptr, nullptr);

	device->CreateTexture(width, height, 0, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, (IDirect3DTexture9**)&alphabetTexture, nullptr);

	D3DLOCKED_RECT lockedRect = {};
	alphabetTexture->LockRect(0, &lockedRect, nullptr, D3DLOCK_DISCARD);

	memcpy(lockedRect.pBits, imageBuffer.data(), totalBytes);

	alphabetTexture->UnlockRect(0);
	fclose(fp);

	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
}

void Renderer::loadAlphabetOffsets()
{
	alphabetOffsets['a'] = { 0, 0 };
	alphabetOffsets['b'] = { 32, 0 };
	alphabetOffsets['c'] = { 64, 0 };
	alphabetOffsets['d'] = { 96, 0 };
	alphabetOffsets['e'] = { 128, 0 };
	alphabetOffsets['f'] = { 160, 0 };
	alphabetOffsets['g'] = { 192, 0 };
	alphabetOffsets['h'] = { 224, 0 };
	alphabetOffsets['i'] = { 256, 0 };
	alphabetOffsets['j'] = { 288, 0 };
	alphabetOffsets['k'] = { 320, 0 };
	alphabetOffsets['l'] = { 352, 0 };
	alphabetOffsets['m'] = { 384, 0 };
	alphabetOffsets['n'] = { 0, 32 };
	alphabetOffsets['o'] = { 32, 32 };
	alphabetOffsets['p'] = { 64, 32 };
	alphabetOffsets['q'] = { 96, 32 };
	alphabetOffsets['r'] = { 128, 32 };
	alphabetOffsets['s'] = { 160, 32 };
	alphabetOffsets['t'] = { 192, 32 };
	alphabetOffsets['u'] = { 224, 32 };
	alphabetOffsets['v'] = { 256, 32 };
	alphabetOffsets['w'] = { 288, 32 };
	alphabetOffsets['x'] = { 320, 32 };
	alphabetOffsets['y'] = { 352, 32 };
	alphabetOffsets['z'] = { 384, 32 };
	alphabetOffsets['0'] = { 0, 64 };
	alphabetOffsets['1'] = { 32, 64 };
	alphabetOffsets['2'] = { 64, 64 };
	alphabetOffsets['3'] = { 96, 64 };
	alphabetOffsets['4'] = { 128, 64 };
	alphabetOffsets['5'] = { 160, 64 };
	alphabetOffsets['6'] = { 192, 64 };
	alphabetOffsets['7'] = { 224, 64 };
	alphabetOffsets['8'] = { 256, 64 };
	alphabetOffsets['9'] = { 288, 64 };

	for (auto pair : alphabetOffsets)
	{
		float x, y;

		if (pair.second.x != 0)
			x = pair.second.x / static_cast<float>(textureWidth);
		else
			x = 0.0f;

		if (pair.second.y != 0)
			y = pair.second.y / static_cast<float>(textureHeight);
		else
			y = 0.0f;

		normalizedAlphabetOffsets[pair.first] = { x, y };
	}
}

void Renderer::createDevice()
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS presentParams = {};

	presentParams.Windowed = true;
	presentParams.BackBufferFormat = D3DFMT_X8R8G8B8;
	presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParams.hDeviceWindow = drawingBuff.hwnd;

	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		drawingBuff.hwnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParams,
		&device);

	FrameData fd = {};
	fd.ballX = 50;
	fd.ballY = 50;
	fd.playerX = 10;
	fd.playerY = 40;

	device->CreateVertexBuffer(
		1000 * sizeof(CustomVertex),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		nullptr);
}

void Renderer::releaseDevice()
{
	alphabetTexture->Release();
	device->Release();
	d3d->Release();
}