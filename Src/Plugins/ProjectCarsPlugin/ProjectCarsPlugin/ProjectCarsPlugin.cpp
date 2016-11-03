// ProjectCarsPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ProjectCars.h"

cProjectCars g_projectCars;


void GetGameName(wchar_t *gameName)
{
	wcscpy(gameName, L";pCARS64.exe");
}


void GetOutputGameName(wchar_t *gameTitleName)
{
	wcscpy(gameTitleName, L";Project CARS¢â");
}


// Game Logo FilePath
void GetGameLogoFilePath(wchar_t *gameLogFilePath)
{
	wcscpy(gameLogFilePath, L";./Plugins/ProjectCars.jpg");
}


int MotionInit(int hWnd, int gameIdx)
{
	return g_projectCars.Init((HWND)hWnd);
}

int MotionUpdateScript()
{
	return g_projectCars.UpdateMotionScript();
}


void MotionEnd()
{
	g_projectCars.End();
}

int MotionUpdate(const float deltaSeconds)
{
	return g_projectCars.Update(deltaSeconds);
}

void MotionClear()
{
	g_projectCars.Clear();
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
