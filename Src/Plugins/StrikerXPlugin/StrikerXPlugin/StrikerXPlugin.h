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


	// 출력 포맷을 지정한다.
	// :output - :format 을 설정한다.
	// outputIndex : :output 이 2개 이상일 경우, 모션 스크립트에서 나오는 순서대로 0,1~
	// 로 정해진다.
	// formatIndex : :format 이 2개 이상일 경우, 모션 스크립트에서 나오는 순서대로 0,1~
	// 로 정해진다.
	DLL_FUNC void MotionSetOutputFormat(int outputIndex, int formatIndex);


	// 모션 스크립트 Varible을 float형으로 변환해서 가져온다.
	DLL_FUNC float MotionGetSymbolFloat(char *symbolName);


	// 모션 스크립트 Varible을 int형으로 변환해서 가져온다.
	DLL_FUNC int MotionGetSymbolInt(char *symbolName);


	// 모션 스크립트 Varible을 float형으로 저장한다.
	DLL_FUNC void MotionSetSymbolFloat(char *symbolName, float val);


	// 모션 스크립트 Varible을 int형으로 저장한다.
	DLL_FUNC void MotionSetSymbolInt(char *symbolName, int val);



	//----------------------------------------------------------------------------
	// 이미지 인식 초기화
	DLL_FUNC int MatchReadScript(char *fileName, char *executeLabel);

	// 이미지 캡쳐, 인식하기
	DLL_FUNC void MatchCaptureAndDetect(char *result);

	// 이미지 캡쳐 지연
	DLL_FUNC void MatchCaptureDelay(int delayMilliSeconds);
}

