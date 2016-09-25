#pragma once


#define DLL_FUNC __declspec(dllexport)

extern "C"
{
	// Game Name To Find Running Game
	DLL_FUNC void GetGameName(char *gameName);


	// To Display VRSim Window
	DLL_FUNC void GetOutputGameName(char *gameTitleName);


	// Read Motion Script and then Start Motion
	// return 0: error, 1:success
	DLL_FUNC int MotionInit(int hWnd);


	// reload Motion Script
	DLL_FUNC int MotionUpdateScript();


	// must call this function, every loop 
	DLL_FUNC int MotionUpdate(const float deltaSeconds);


	// Finish Motion
	DLL_FUNC void MotionEnd();


	// remove motion data
	DLL_FUNC void MotionClear();
}

