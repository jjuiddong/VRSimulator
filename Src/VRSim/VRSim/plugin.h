//
// 2016-07-10, jjuiddong
// plugin function
//
#pragma once


// Plugin DLL Function
extern "C"
{
	typedef void(__cdecl *__GetGameName) (wchar_t *gameName);
	typedef void(__cdecl *__GetOutputGameName) (wchar_t *gameName);
	typedef void(__cdecl *__GetGameLogoFilePath) (wchar_t *gameLogoPath);
	typedef int(__cdecl *__MotionInit) (int hwnd, int gameIdx);
	typedef int(__cdecl *__MotionUpdateScript) ();
	typedef int (__cdecl *__MotionUpdate) (const float deltaSeconds);
	typedef void (__cdecl *__MotionSetSymbol) (char *symbol, const float data);
	typedef void(__cdecl *__MotionSetOutputFormat) (const int formatIndex);
	typedef float(__cdecl *__MotionGetSymbolFloat)(char*);
	typedef void(__cdecl *__MotionEnd) ();
	typedef void(__cdecl *__MotionClear) ();
}


// Plugin Information Structure
struct sPluginInfo
{
	__GetGameName GetGameName;
	__GetOutputGameName GetOutputGameName;
	__GetGameLogoFilePath GetGameLogoFilePath;
	__MotionInit MotionInit;
	__MotionUpdateScript MotionUpdateScript;
	__MotionUpdate MotionUpdate;
	__MotionSetSymbol MotionSetSymbol;
	__MotionGetSymbolFloat MotionGetSymbolFloat; // not used
	__MotionSetOutputFormat MotionSetOutputFormat;
	__MotionEnd MotionEnd;
	__MotionClear MotionClear;

	HMODULE lib;
	vector<wstring> gameNames;
	vector<wstring> outputGameNames;
	vector<wstring> gameLogoFiles;

	bool Load(const string &dllFileName);
};

