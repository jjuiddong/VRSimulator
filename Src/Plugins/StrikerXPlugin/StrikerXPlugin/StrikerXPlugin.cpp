// StrikerXPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "StrikerX.h"

cStrikerX g_strikerX;



void GetGameName(wchar_t *gameName)
{
	wcscpy(gameName, L";Bugwar.exe");
}


void GetOutputGameName(wchar_t *gameTitleName)
{
	wcscpy(gameTitleName, L";StrikerX");
}


// Game Logo FilePath
void GetGameLogoFilePath(wchar_t *gameLogFilePath)
{
	wcscpy(gameLogFilePath, L";./Plugins/StrikerX.jpg");
}


int MotionInit(int hWnd, int gameIdx)
{
	return g_strikerX.Init((HWND)hWnd);
}

int MotionUpdateScript()
{
	return g_strikerX.UpdateMotionScript();
}


void MotionEnd()
{
	g_strikerX.End();
}

int MotionUpdate(const float deltaSeconds)
{
	return g_strikerX.Update(deltaSeconds);
}

void MotionClear()
{
	g_strikerX.Clear();
	motion::cController2::Release();
}


// set symbol data
void MotionSetSymbol(const char *key, const float data)
{
	script::g_symbols[key].type = script::FIELD_TYPE::T_FLOAT;
	script::g_symbols[key].fVal = data;
}

// set output format index
void MotionSetOutputFormat(const int formatIndex)
{
	motion::cController2::Get()->SetOutputFormat(0, formatIndex);
}
