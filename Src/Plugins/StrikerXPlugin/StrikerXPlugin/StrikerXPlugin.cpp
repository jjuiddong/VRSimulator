// StrikerXPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


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
	return motion::cController2::Get()->Init((HWND)hWnd, "strikerx.txt");
}

int MotionUpdate(const float deltaSeconds)
{
	return motion::cController2::Get()->Update(deltaSeconds);
}

void MotionEnd()
{

}

void MotionClear()
{
	motion::cController2::Release();
	cvproc::imagematch::cMatchManager::Release();
}

int MotionStart()
{
	return motion::cController2::Get()->Start();
}

int MotionStop()
{
	return motion::cController2::Get()->Stop();
}

void MotionSetOutputFormat(int outputIndex, int formatIndex)
{
	motion::cController2::Get()->SetOutputFormat(outputIndex, formatIndex);
}

float MotionGetSymbolFloat(char *symbolName)
{
	return script::g_symbols[symbolName].fVal;
}

int MotionGetSymbolInt(char *symbolName)
{
	return script::g_symbols[symbolName].iVal;
}

void MotionSetSymbolFloat(char *symbolName, float val)
{
	script::g_symbols[symbolName].type = script::FIELD_TYPE::T_FLOAT;
	script::g_symbols[symbolName].fVal = val;
}

void MotionSetSymbolInt(char *symbolName, int val)
{
	script::g_symbols[symbolName].type = script::FIELD_TYPE::T_INT;
	script::g_symbols[symbolName].iVal = val;
}

int MatchReadScript(char *fileName, char *executeLabel)
{
	return cvproc::imagematch::cMatchManager::Get()->Init(fileName, executeLabel);
}

void MatchCaptureAndDetect(char *result)
{
	string str = cvproc::imagematch::cMatchManager::Get()->CaptureAndDetect();
	strcpy(result, str.c_str());
}

void MatchCaptureDelay(int delayMilliSeconds)
{
	cvproc::imagematch::cMatchManager::Get()->CaptureDelay(delayMilliSeconds);
}

