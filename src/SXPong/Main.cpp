// PongReworked.cpp : Defines the entry point for the application.
//

#define WIN32_LEAN_AND_MEAN 
#define _WINSOCKAPI_ 
#define _CRTDBG_MAP_ALLOC
#include "stdafx.h"
#include <stdlib.h>
#include <crtdbg.h>
#include "Constants.hpp"
#include "Game.hpp"
#include "SxConnector.hpp"
#include "guicon.h"
#include "Networker.hpp"
#include <thread>
#include <chrono>

// Global Variables:
Game *game = nullptr;
SxConnector * sxConnector = nullptr;
HWND hWnd, sxWindow = nullptr;
HINSTANCE hInst;								// current instance
LPCWSTR szTitle = L"Pong";								// The title bar text
LPCWSTR szWindowClass = L"PongWindow";						// the main window class name
LPCWSTR szChildWindowClass = L"ChildWindow";
long timeNow, timeThen;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	SeaCrossMessageHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	GameMessageHandler(HWND, UINT, WPARAM, LPARAM);
void				RegisterRawInputDevices(HWND);
HWND				createChildWindow(HWND hWnd);

// SeaCross Message declarations
UINT SXWM_CREATE = RegisterWindowMessage(L"SxSdk_Msg_NameBase 101");
UINT SXWM_DESTROY = RegisterWindowMessage(L"SxSdk_Msg_NameBase 102");
UINT SXWM_RESIZEDMSG = RegisterWindowMessage(L"SxSdk_Msg_NameBase 104");

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#ifdef _DEBUG

	RedirectIOToConsole();

#endif

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MyRegisterClass(hInstance);
	
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	sxConnector = new SxConnector(hWnd);
	
	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			timeNow = std::chrono::duration_cast<std::chrono::milliseconds>
				(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		if (sxWindow != nullptr)
			game->run();
		
	} 

	delete(game);
	delete(sxConnector);
	_CrtDumpMemoryLeaks();
	return static_cast<int>(msg.wParam);
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= SeaCrossMessageHandler;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszClassName	= szWindowClass;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, (WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN) &~WS_THICKFRAME &~WS_MAXIMIZEBOX,
      0, 0, 0, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK SeaCrossMessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HRESULT hResult;

	if (message == SXWM_CREATE)
	{
		sxConnector->setWinId(wParam);
		sxWindow = sxConnector->obtainWinHandle();
		HWND childWindow = createChildWindow(sxWindow);
		RegisterRawInputDevices(childWindow);
		game = new Game(childWindow);
	}
	else if (message == SXWM_DESTROY)
	{
		PostQuitMessage(0);
	}
	else if (message == SXWM_RESIZEDMSG)
	{
		
	}

	switch (message)
	{
	case WM_INPUT:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK GameMessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HRESULT hResult;

	switch (message)
	{
	case WM_NCHITTEST:
		return 0;
		break;
	case WM_MOUSEACTIVATE:
		return 0;
		break;
	case WM_SETCURSOR:
		return 0;
		break;
	case WM_CONTEXTMENU:
		return 0;
		break;
	case WM_INPUT:
	{
		if (sxWindow != nullptr)
		{
			// Determine how big the buffer should be
			UINT bufferSize;
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));

			// Create a buffer of the correct size - but see note below
			BYTE *buffer = new BYTE[bufferSize];

			// Call the function again, this time with the buffer to get the data
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (LPVOID)buffer, &bufferSize, sizeof(RAWINPUTHEADER));

			RAWINPUT *raw = (RAWINPUT*)buffer;
			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				game->handleRawMouseInput(raw);
			}
		}
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case LEFTPLAYER_BUTTON_ID:
			DestroyWindow((HWND)lParam);
			DestroyWindow(GetDlgItem(hWnd, RIGHTPLAYER_BUTTON_ID));
			game->handlePushButtonClick(LEFTPLAYER_BUTTON_ID);
			break;
		case RIGHTPLAYER_BUTTON_ID:
			DestroyWindow((HWND)lParam);
			DestroyWindow(GetDlgItem(hWnd, LEFTPLAYER_BUTTON_ID));
			game->handlePushButtonClick(RIGHTPLAYER_BUTTON_ID);
			break;
		case MAINMENU_BUTTON_ID:
			DestroyWindow((HWND)lParam);
			DestroyWindow(GetDlgItem(hWnd, RESTART_BUTTON_ID));
			game->handlePushButtonClick(MAINMENU_BUTTON_ID);
			break;
		case RESTART_BUTTON_ID:
			DestroyWindow((HWND)lParam);
			DestroyWindow(GetDlgItem(hWnd, MAINMENU_BUTTON_ID));
			game->handlePushButtonClick(RESTART_BUTTON_ID);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		int returnCode = DefWindowProc(hWnd, message, wParam, lParam);
		timeThen = timeNow;
		timeNow = std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		int deltaTime = timeNow - timeThen;
		if (deltaTime > FPS / 1000)
			std::cout << "Message: " << std::to_string(message) << " Time: " << std::to_string(deltaTime) << std::endl;
		return returnCode;
	}
	timeThen = timeNow;
	timeNow = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	int deltaTime = timeNow - timeThen;
	if (deltaTime > FPS / 1000)
		std::cout << "Message: " << std::to_string(message) << " Time: " << std::to_string(deltaTime) << std::endl;
	return 0;
}

void RegisterRawInputDevices(HWND hWnd)
{
	RAWINPUTDEVICE Rid[2];

	// Keyboard
	Rid[0].usUsagePage = 1;
	Rid[0].usUsage = 6;
	Rid[0].dwFlags = 0;
	Rid[0].hwndTarget = hWnd;

	// Mouse
	Rid[1].usUsagePage = 1;
	Rid[1].usUsage = 2;
	Rid[1].dwFlags = 0;
	Rid[1].hwndTarget = hWnd;

	if (RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE)) == FALSE)
	{
		printf("Failed to register raw input devices\n");
	}	
}

HWND createChildWindow(HWND hWnd)
{
	RECT parrentRect = {};
	GetClientRect(hWnd, &parrentRect);
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = GameMessageHandler;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = szChildWindowClass;
	RegisterClassEx(&wcex);

	HWND childWindow = CreateWindow(szChildWindowClass,
		NULL,
		WS_CHILD | WS_VISIBLE,
		0,
		0,
		parrentRect.right,
		parrentRect.bottom,
		hWnd,
		nullptr,
		hInstance,
		nullptr
		);

	if (childWindow)
		return childWindow;
	else
		return nullptr;
}
