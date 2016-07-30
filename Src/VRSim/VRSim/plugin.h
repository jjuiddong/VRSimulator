//
// 2016-07-10, jjuiddong
// �÷����ο� ���õ� ������ �����Ѵ�.
//
#pragma once


// �÷����� DLL �Լ� 
extern "C"
{
	typedef void(__cdecl *__GetGameName) (char *gameName);
	typedef void(__cdecl *__GetOutputGameName) (char *gameName);
	typedef int(__cdecl *__MotionInit) (int hwnd);
	typedef int(__cdecl *__MotionUpdateScript) ();
	typedef int (__cdecl *__MotionUpdate) (const float deltaSeconds);
	typedef void(__cdecl *__MotionEnd) ();
	typedef void(__cdecl *__MotionClear) ();
	typedef float(__cdecl *__MotionGetSymbolFloat)(char*);
}


// �÷����� ���� ����ü
struct sPluginInfo
{
	__GetGameName GetGameName;
	__GetOutputGameName GetOutputGameName;
	__MotionInit MotionInit;
	__MotionUpdateScript MotionUpdateScript;
	__MotionUpdate MotionUpdate;
	__MotionEnd MotionEnd;
	__MotionClear MotionClear;
	__MotionGetSymbolFloat MotionGetSymbolFloat;

	HMODULE lib;
	char gameName[64];
	char outputGameName[64];

	bool Load(const string &dllFileName);
};

