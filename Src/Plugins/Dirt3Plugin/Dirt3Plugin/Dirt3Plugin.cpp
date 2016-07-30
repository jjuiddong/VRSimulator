// Dirt3Plugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Dirt3.h"

cDirt3 g_dirt3;

void GetGameName(char *gameName)
{
	strcpy(gameName, "DiRT 3");
}


// 매니저에 출력할 게임 타이틀 이름
void GetOutputGameName(char *gameTitleName)
{
	strcpy(gameTitleName, "DiRT 3");
}


int MotionInit(int hWnd)
{
	return g_dirt3.Init((HWND)hWnd);
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
	cvproc::imagematch::cMatchManager::Release();
}
