#include "stdafx.h"
#include "SxConnector.hpp"

void SxConnector::setWinId(int winId)
{
	this->winID = winId;
}

HWND SxConnector::obtainWinHandle()
{
	getWindowHandle();
	if (handleObtained)
		return sxSuppliedWindowHandle;
	else
		return 0;
}

void SxConnector::getWindowHandle()
{
	sxSuppliedWindowHandle = (HWND)SXSDK_QR_GetWindowHandle(UCI, winID);
	if (!sxSuppliedWindowHandle)
	{
		handleObtained = false;
		std::cout << "Failed to obtain windowhandle from SX\n";
	}
	else
	{
		handleObtained = true;
	}
}

void SxConnector::winIDDestroyed()
{
	winIDObtained = false;
	handleObtained = false;
}

void SxConnector::isCorrectWinID(WPARAM wParam, LPARAM lParam)
{
	if (lParam == GUID)
	{
		std::cout << "SX WinID recieved\n";
		winID = wParam;
		winIDObtained = true;
	}
}

void SxConnector::initializeConnections()
{
	loadDll();
	if (dllLoaded())
	{
		loadFunctions();
		if (functionsLoaded())
		{
			switch (SXSDK_Connect(appID, unlockCode, (uint32_t)messageWindow, &UCI))
			{
			case 0:
				std::cout << "Application connected to SDK\n";
				break;
			case 1:
				std::cout << "Connection to SDK failed: Invalid ID\n";
				break;
			case 2:
				std::cout << "Connection to SDK failed: Invalid Unlock Code\n";
				break;
			case 4:
				std::cout << "Error: SeaCross is not executing\n";
				break;
			case 8:
				std::cout << "Error: Number Of Active UCI Exceeds 32\n";
				break;
			}
		}
	}
}

void SxConnector::loadFunctions()
{
	SXSDK_Connect = reinterpret_cast<SxSdk_Connect>(GetProcAddress(SXLib, "SxSdk_Connect"));
	SXSDK_Disconnect = reinterpret_cast<SxSdk_Disconnect>(GetProcAddress(SXLib, "SxSdk_Disconnect"));
	SXSDK_WT_Register = reinterpret_cast<SxSdk_WT_Register>(GetProcAddress(SXLib, "SxSdk_WT_Register"));
	SXSDK_WT_Remove = reinterpret_cast<SxSdk_WT_Remove>(GetProcAddress(SXLib, "SxSdk_WT_Remove"));
	SXSDK_QR_GetWinState = reinterpret_cast<SxSdk_QR_GetWinState>(GetProcAddress(SXLib, "SxSdk_QR_GetWinState"));
	SXSDK_QR_GetDayNightMode = reinterpret_cast<SxSdk_QR_GetDayNightMode>(GetProcAddress(SXLib, "SxSdk_QR_GetDayNightMode"));
	SXSDK_QR_GetWindowHandle = reinterpret_cast<SxSdk_QR_GetWindowHandle>(GetProcAddress(SXLib, "SxSdk_QR_GetWindowHandle"));
	SXSDK_WT_Get = reinterpret_cast<SxSdk_WT_Get>(GetProcAddress(SXLib, "SxSdk_WT_Get"));
}

bool SxConnector::functionsLoaded()
{
	if (!SXSDK_Connect || !SXSDK_Disconnect || !SXSDK_QR_GetWindowHandle)
	{
		std::cout << "Functions failed to load\n";
		return false;
	}
	else
		return true;
}

void SxConnector::loadDll()
{
	SXLib = LoadLibrary(TEXT("SxSdkC.dll"));
}

bool SxConnector::dllLoaded()
{
	if (!SXLib)
		return false;
	else
		return true;
}

bool SxConnector::unregisterWindow()
{
	if (SXSDK_WT_Remove(UCI, GUID))
		return true;
	return false;
}

void SxConnector::registerWindow()
{
	// Registering button
	mainButton.Id = mainButtonID;
	mainButton.CmdPbFlags = 1;
	mainButton.PbBitmap = tPbBmp{};
	char buttonTitle[] = "Pong";
	mainButton.CmdPbText[0] = (char)strlen(buttonTitle);
	_snprintf(mainButton.CmdPbText + 1, sizeof mainButton.CmdPbText, "%s", buttonTitle);

	// Registering Window
	char windowName[] = "SXPong";
	char hint[] = "The classic Pong from 1976, now ported to SeaCross!";
	winTypeDef->Name[0] = (char)strlen(windowName);
	winTypeDef->Hint[0] = (char)strlen(hint);
	_snprintf(winTypeDef->Name + 1, sizeof winTypeDef->Name - 1, "%s", windowName);
	_snprintf(winTypeDef->Hint + 1, sizeof winTypeDef->Hint - 1, "%s", hint);
	
	winTypeDef->DefSize = sizeof(tWinTypeDef);
	winTypeDef->Version = 1;
	winTypeDef->Flags = 1;
	winTypeDef->MainButton = mainButton;
	winTypeDef->NoOfFuncButtons = 0;

	switch (SXSDK_WT_Register(UCI, GUID, winTypeDef))
	{
	case 0:
		std::cout << "The Window Type with GUID was registered\n";
		break;
	case 1:
		std::cout << "The Window Type with GUID was redefined\n";
		break;
	case 2:
		std::cout << "Window registration failed: The UCI is invalid\n";
		break;
	case 4:
		std::cout << "Window registration failed: Invalid pWinTypePointer\n";
		break;
	case 8:
		std::cout << "Window registration failed: Invalid DefSize\n";
		break;
	case 16:
		std::cout << "Window registration failed: Invalid version\n";
		break;
	case 32:
		std::cout << "Window registration failed: No Of Defined Function Push Buttons exceeds 20\n";
		break;
	case 64:
		std::cout << "Window registration failed: One or more Push Button definitions is invalid\n";
		break;
	}
}


