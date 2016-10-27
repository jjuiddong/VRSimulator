// Dirt3Plugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Dirt3.h"

cDirt3 g_dirt3;


void GetGameName(wchar_t *gameName)
{
	wcscpy(gameName, L";DiRT 3;GRID Autosport;DiRT Showdown");
}


void GetOutputGameName(wchar_t *gameTitleName)
{
	wcscpy(gameTitleName, L";DiRT 3;GRID Autosport;DiRT Showdown");
}


// Game Logo FilePath
void GetGameLogoFilePath(wchar_t *gameLogFilePath)
{
	wcscpy(gameLogFilePath, L";./Plugins/Dirt3.jpg;./Plugins/Grid.jpg;./Plugins/Dirt Showdown.jpg");
}


int MotionInit(int hWnd, int gameIdx)
{
	return g_dirt3.Init((HWND)hWnd, gameIdx);
}

int MotionUpdateScript()
{
	return g_dirt3.UpdateMotionScript();
}


void MotionEnd()
{
	g_dirt3.End();
}

int MotionUpdate(const float deltaSeconds)
{
	return g_dirt3.Update(deltaSeconds); 
}

void MotionClear()
{
 	g_dirt3.Clear();
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
