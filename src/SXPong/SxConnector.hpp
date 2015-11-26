#pragma once

#include "Game.hpp"
#include "Windows.h"
#include "SxTypes.hpp"
#include <cstdint>
#include <string.h>
#include <cstdio>
#include <iostream>

// Function pointer definitions
typedef int(*SxSdk_Connect)(const char * id, const char * UnlockCode, uint32_t WindowHandle, uint32_t * UCI);
typedef void(*SxSdk_Disconnect)(unsigned UCI);
typedef int(*SxSdk_WT_Register)(uint32_t UCI, uint32_t GUID, const tWinTypeDef *pWinTypeDef);
typedef int(*SxSdk_WT_Remove)(uint32_t UCI, uint32_t GUID);
typedef int(*SxSdk_QR_GetWinState)(uint32_t UCI, uint32_t Winid, tWinState * pWinState);
typedef int(*SxSdk_QR_GetDayNightMode)(uint32_t UCI);
typedef int(*SxSdk_QR_GetWindowHandle)(uint32_t UCI, uint32_t Winid);
typedef int(*SxSdk_WT_Get)(uint32_t UCI, uint32_t GUID, const tWinTypeDef * pWinTypeDef);

class SxConnector
{
public:
	SxConnector(HWND window) :
		appID("KDA"),
		unlockCode("A6BF34D123AAFB658C"),
		messageWindow(window),
		winTypeDef(new tWinTypeDef()),
		GUID(45),
		mainButtonID(15),
		mainButton(),
		winIDObtained(false),
		handleObtained(false),
		winID(0)
	{
		initializeConnections();

		//registerWindow();
		//unregisterWindow();
		
	}

	~SxConnector()
	{
		delete(winTypeDef);
	}

	void isCorrectWinID(WPARAM wParam, LPARAM lParam);
	void winIDDestroyed();
	HWND obtainWinHandle();
	void getWindowHandle();
	void setWinId(int);

protected:
	// Variables
	uint32_t       UCI;
	uint32_t       GUID;
	uint32_t       winID;
	const uint32_t mainButtonID;
	const char    *appID;
	const char    *unlockCode;
	tWinTypeDef   *winTypeDef;
	tPushButton    mainButton;
	HMODULE        SXLib;
	HWND           messageWindow;
	HWND           sxSuppliedWindowHandle;
	bool           winIDObtained;
	bool           handleObtained;
	bool           contextsCreated;

	// Functions
	void initializeConnections();
	void loadDll();
	bool dllLoaded();
	void loadFunctions();
	bool functionsLoaded();
	void registerWindow();
	bool unregisterWindow();
	
	// SxSdk Functions
	SxSdk_Connect SXSDK_Connect;
	SxSdk_Disconnect SXSDK_Disconnect;
	SxSdk_WT_Register SXSDK_WT_Register;
	SxSdk_WT_Remove SXSDK_WT_Remove;
	SxSdk_QR_GetWinState SXSDK_QR_GetWinState;
	SxSdk_QR_GetDayNightMode SXSDK_QR_GetDayNightMode;
	SxSdk_QR_GetWindowHandle SXSDK_QR_GetWindowHandle;
	SxSdk_WT_Get SXSDK_WT_Get;
};

