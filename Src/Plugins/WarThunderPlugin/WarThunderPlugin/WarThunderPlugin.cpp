// WarThunderPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "WarThunder.h"

cWarThunder g_warThunder;


void GetGameName(char *gameName)
{
	strcpy(gameName, "War Thunder - In battle");
}


void GetOutputGameName(char *gameTitleName)
{
	strcpy(gameTitleName, "War Thunder");
}


int MotionInit(int hWnd)
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
