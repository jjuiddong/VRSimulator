// StrikerXPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "StrikerX.h"

cStrikerX g_strikerX;


void GetGameName(char *gameName)
{
	strcpy(gameName, "StrikerX");
}


// 매니저에 출력할 게임 타이틀 이름
void GetOutputGameName(char *gameTitleName)
{
	strcpy(gameTitleName, "StrikerX");
}


int MotionInit(int hWnd)
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
