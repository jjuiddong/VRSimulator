//
// 2016-07-10, jjuiddong
// plugin fuction
//
#pragma once


// Plugin DLL Function
extern "C"
{
	typedef void(__cdecl *__GetGameName) (char *gameName);
	typedef void(__cdecl *__GetOutputGameName) (char *gameName);
	typedef int(__cdecl *__MotionInit) (int hwnd);
	typedef int(__cdecl *__MotionUpdateScript) ();
	typedef int (__cdecl *__MotionUpdate) (const float deltaSeconds);
	typedef void (__cdecl *__MotionSetSymbol) (char *symbol, const float data);
	typedef void(__cdecl *__MotionSetOutputFormat) (const int formatIndex);
	typedef void(__cdecl *__MotionEnd) ();
	typedef void(__cdecl *__MotionClear) ();
	typedef float(__cdecl *__MotionGetSymbolFloat)(char*);
}


// Plugin Information Structure
struct sPluginInfo
{
	__GetGameName GetGameName;
	__GetOutputGameName GetOutputGameName;
	__MotionInit MotionInit;
	__MotionUpdateScript MotionUpdateScript;
	__MotionUpdate MotionUpdate;
	__MotionSetSymbol MotionSetSymbol;
	__MotionSetOutputFormat MotionSetOutputFormat;
	__MotionEnd MotionEnd;
	__MotionClear MotionClear;
	__MotionGetSymbolFloat MotionGetSymbolFloat;

	HMODULE lib;
	char gameName[64];
	char outputGameName[64];

	bool Load(const string &dllFileName);
};

