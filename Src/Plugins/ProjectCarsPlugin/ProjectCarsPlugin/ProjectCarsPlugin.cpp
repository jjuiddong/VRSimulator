// ProjectCarsPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ProjectCars.h"

cProjectCars g_projectCars;


void GetGameName(char *gameName)
{
	strcpy(gameName, "Project CARS¢â");
}


void GetOutputGameName(char *gameTitleName)
{
	strcpy(gameTitleName, "Project CARS¢â");
}


int MotionInit(int hWnd)
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
