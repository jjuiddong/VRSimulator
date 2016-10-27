#pragma once


#define DLL_FUNC __declspec(dllexport)

extern "C"
{
	// Game Name To Find Running Game
	DLL_FUNC void GetGameName(wchar_t *gameName);


	// To Display VRSim Window
	DLL_FUNC void GetOutputGameName(wchar_t *gameTitleName);


	// Game Logo FilePath
	DLL_FUNC void GetGameLogoFilePath(wchar_t *gameLogFilePath);


	// Read Motion Script and then Start Motion
	// return 0: error, 1:success
	DLL_FUNC int MotionInit(int hWnd, int gameIdx);


	// 모션 스크립트 파일을 다시 읽는다.
	DLL_FUNC int MotionUpdateScript();


	// 매 루프때마다 호출해야 하는 함수
	// deltaSeconds : 1초가 지나면 1
	DLL_FUNC int MotionUpdate(const float deltaSeconds);

	// set symbol data
	DLL_FUNC void MotionSetSymbol(const char *key, const float data);


	// set output format index
	DLL_FUNC void MotionSetOutputFormat(const int formatIndex);


	// 모션 종료
	DLL_FUNC void MotionEnd();


	// 초기화
	DLL_FUNC void MotionClear();
}

