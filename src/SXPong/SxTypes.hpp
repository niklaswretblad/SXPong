#pragma once

#include <stdint.h>

struct tPbBmp
{
	uint32_t Width;
	uint32_t Height;
	uint32_t Pixels[4096];
	uint32_t Disabled[4096];
	uint32_t MaskColor;
};

struct tPushButton
{
	uint32_t Id;
	uint32_t CmdPbFlags;
	char CmdPbText[256];
	tPbBmp PbBitmap;
	char Hint[256];
};

struct tWinTypeDef
{
	uint32_t DefSize;
	uint32_t Version;
	uint32_t Flags;
	char Name[256];
	char Hint[256];
	tPushButton MainButton;
	uint32_t NoOfFuncButtons;
	tPushButton FuncButtons[20];
};

struct tWinState
{
	uint32_t size;
	uint32_t version;
	uint32_t flags;
	uint32_t left;
	uint32_t top;
	uint32_t width;
	uint32_t height;
};

