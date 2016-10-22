// TestPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "TestPlugin.h"


void GetGameName(char *gameName)
{
	strcpy(gameName, "Test");
}


void GetOutputGameName(char *gameTitleName)
{
	strcpy(gameTitleName, "Display Test");
}


int MotionInit(int hWnd)
{
//	return g_dirt3.Init((HWND)hWnd);
	return 1;
}

int MotionUpdateScript()
{
//	return g_dirt3.UpdateMotionScript();
	return 1;
}


void MotionEnd()
{
//	g_dirt3.End();
}

int MotionUpdate(const float deltaSeconds)
{
//	return g_dirt3.Update(deltaSeconds);
	return 1;
}

void MotionClear()
{
// 	g_dirt3.Clear();
// 	motion::cController2::Release();
}
