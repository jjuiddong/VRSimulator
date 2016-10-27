// NoLimit2Plugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "NoLimit2.h"

cNoLimits2 g_noLimits2;


void GetGameName(wchar_t *gameName)
{
	wcscpy(gameName, L";NoLimits 2");
}


void GetOutputGameName(wchar_t *gameTitleName)
{
	wcscpy(gameTitleName, L";NoLimits 2");
}


// Game Logo FilePath
void GetGameLogoFilePath(wchar_t *gameLogFilePath)
{
	wcscpy(gameLogFilePath, L";./Plugins/NoLimits 2.jpg");
}


int MotionInit(int hWnd, int gameIdx)
{
	return g_noLimits2.Init((HWND)hWnd, gameIdx);
}

int MotionUpdateScript()
{
	return g_noLimits2.UpdateMotionScript();
}


void MotionEnd()
{
	g_noLimits2.End();
}

int MotionUpdate(const float deltaSeconds)
{
	return g_noLimits2.Update(deltaSeconds);
}

void MotionClear()
{
	g_noLimits2.Clear();
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
