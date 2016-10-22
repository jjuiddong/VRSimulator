// Dirt3Plugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Dirt3.h"

cDirt3 g_dirt3;


void GetGameName(char *gameName)
{
	strcpy(gameName, "DiRT 3");
}


void GetOutputGameName(char *gameTitleName)
{
	strcpy(gameTitleName, "DiRT 3");
}


int MotionInit(int hWnd)
{
	return g_dirt3.Init((HWND)hWnd);
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
