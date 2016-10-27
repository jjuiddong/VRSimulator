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


	// ��� ��ũ��Ʈ ������ �ٽ� �д´�.
	DLL_FUNC int MotionUpdateScript();


	// �� ���������� ȣ���ؾ� �ϴ� �Լ�
	// deltaSeconds : 1�ʰ� ������ 1
	DLL_FUNC int MotionUpdate(const float deltaSeconds);

	// set symbol data
	DLL_FUNC void MotionSetSymbol(const char *key, const float data);


	// set output format index
	DLL_FUNC void MotionSetOutputFormat(const int formatIndex);


	// ��� ����
	DLL_FUNC void MotionEnd();


	// �ʱ�ȭ
	DLL_FUNC void MotionClear();
}

