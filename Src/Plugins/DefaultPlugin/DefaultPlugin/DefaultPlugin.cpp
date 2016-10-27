// DefaultPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

using namespace motion;

void GetGameName(wchar_t *gameName)
{
	wcscpy(gameName, L";Default Plugin, does not match any window name");
}


void GetOutputGameName(wchar_t *gameTitleName)
{
	wcscpy(gameTitleName, L"; ");
}


// Game Logo FilePath
void GetGameLogoFilePath(wchar_t *gameLogFilePath)
{
	wcscpy(gameLogFilePath, L"; ");
}


int MotionInit(int hWnd, int gameIdx)
{
	int result = motion::cController2::Get()->Init((HWND)hWnd, "Plugins/default.txt")? 1 : 0;
	motion::cController2::Get()->Start();
	return result;
}

int MotionUpdateScript()
{
	return motion::cController2::Get()->Reload("Plugins/default.txt");
}


void MotionEnd()
{
}

int MotionUpdate(const float deltaSeconds)
{
	return cController2::Get()->Update(deltaSeconds);
}

void MotionClear()
{
	motion::cController2::Release();
}

// set symbol data
void MotionSetSymbol(const char *key, const float data)
{
}

// set output format index
void MotionSetOutputFormat(const int formatIndex)
{
	motion::cController2::Get()->SetOutputFormat(0, formatIndex);
}
