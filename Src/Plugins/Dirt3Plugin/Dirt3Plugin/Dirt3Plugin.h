#pragma once


#define DLL_FUNC __declspec(dllexport)

extern "C"
{
	// 게임 이름
	DLL_FUNC void GetGameName(char *gameName);


	// 매니저에 출력할 게임 타이틀 이름
	DLL_FUNC void GetOutputGameName(char *gameTitleName);


	// 모션 스크립트 파일을 읽는 함수
	// 성공하면 1을 리턴하고, 그렇지 않으면 0을 리턴한다.
	DLL_FUNC int MotionInit(int hWnd);


	// 매 루프때마다 호출해야 하는 함수
	// deltaSeconds : 1초가 지나면 1
	DLL_FUNC int MotionUpdate(const float deltaSeconds);

	// 모션 종료
	DLL_FUNC void MotionEnd();

	// 초기화
	DLL_FUNC void MotionClear();
}

