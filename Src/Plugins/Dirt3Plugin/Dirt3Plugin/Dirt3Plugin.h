#pragma once


#define DLL_FUNC __declspec(dllexport)

extern "C"
{
	// ���� �̸�
	DLL_FUNC void GetGameName(char *gameName);


	// �Ŵ����� ����� ���� Ÿ��Ʋ �̸�
	DLL_FUNC void GetOutputGameName(char *gameTitleName);


	// ��� ��ũ��Ʈ ������ �д� �Լ�
	// �����ϸ� 1�� �����ϰ�, �׷��� ������ 0�� �����Ѵ�.
	DLL_FUNC int MotionInit(int hWnd);


	// �� ���������� ȣ���ؾ� �ϴ� �Լ�
	// deltaSeconds : 1�ʰ� ������ 1
	DLL_FUNC int MotionUpdate(const float deltaSeconds);

	// ��� ����
	DLL_FUNC void MotionEnd();

	// �ʱ�ȭ
	DLL_FUNC void MotionClear();
}

