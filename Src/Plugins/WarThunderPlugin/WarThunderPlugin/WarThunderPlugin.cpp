// WarThunderPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "WarThunder.h"

cWarThunder g_warThunder;


void GetGameName(wchar_t *gameName)
{
	wcscpy(gameName, L";War Thunder;War Thunder - In battle");
}


void GetOutputGameName(wchar_t *gameTitleName)
{
	wcscpy(gameTitleName, L";War Thunder;War Thunder");
}


// Game Logo FilePath
void GetGameLogoFilePath(wchar_t *gameLogFilePath)
{
	wcscpy(gameLogFilePath, L";./Plugins/WarThunder.jpg;./Plugins/WarThunder.jpg");
}


int MotionInit(int hWnd, int gameIdx)
{
	return g_warThunder.Init((HWND)hWnd);
}

int MotionUpdateScript()
{
	return g_warThunder.UpdateMotionScript();
}


void MotionEnd()
{
	g_warThunder.End();
}

int MotionUpdate(const float deltaSeconds)
{
	return g_warThunder.Update(deltaSeconds);
}

void MotionClear()
{
	g_warThunder.Clear();
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
