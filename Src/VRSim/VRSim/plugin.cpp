
#include "stdafx.h"
#include "plugin.h"



void GetStringWithDelimiter( void (__cdecl *func) (wchar_t *),  vector<wstring> &out)
{
	if (!func)
		return;

	wchar_t buffer[256];
	ZeroMemory(buffer, sizeof(buffer));
	func(buffer);
	
	// first character is delimeter
	wstring delimeter;
	delimeter += buffer[0];
	wtokenizer(buffer + 1, delimeter, L"", out);
}


bool sPluginInfo::Load(const string &dllFileName)
{
	// load dll file
	lib = LoadLibraryA(dllFileName.c_str());
	if (lib)
	{
		// bind function from dll file
		GetGameName = (__GetGameName)GetProcAddress(lib, "GetGameName");
		GetOutputGameName = (__GetOutputGameName)GetProcAddress(lib, "GetOutputGameName");
		GetGameLogoFilePath = (__GetGameLogoFilePath)GetProcAddress(lib, "GetGameLogoFilePath");
		MotionInit = (__MotionInit)GetProcAddress(lib, "MotionInit");
		MotionUpdateScript = (__MotionUpdateScript)GetProcAddress(lib, "MotionUpdateScript");
		MotionUpdate = (__MotionUpdate)GetProcAddress(lib, "MotionUpdate");
		MotionSetSymbol = (__MotionSetSymbol)GetProcAddress(lib, "MotionSetSymbol");
		MotionSetOutputFormat = (__MotionSetOutputFormat)GetProcAddress(lib, "MotionSetOutputFormat");
		MotionEnd = (__MotionEnd)GetProcAddress(lib, "MotionEnd");
		MotionClear = (__MotionClear)GetProcAddress(lib, "MotionClear");

		GetStringWithDelimiter(GetGameName, gameNames);
		GetStringWithDelimiter(GetOutputGameName, outputGameNames);
		GetStringWithDelimiter(GetGameLogoFilePath, gameLogoFiles);

		dbg::Log("dll load %s \n", dllFileName.c_str());

		if ((gameNames.size() != outputGameNames.size()) ||
			(gameNames.size() != gameLogoFiles.size()))
		{
			dbg::Log("error occur, game name, output gamename, game logo path size different \n");
		}

		for each (auto &str in gameNames)
 			dbg::Log("    - game name = %s \n", wstr2str(str).c_str());
		for each (auto &str in outputGameNames)
			dbg::Log("    - output game name = %s \n", wstr2str(str).c_str());
		for each (auto &str in gameLogoFiles)
			dbg::Log("    - game logo file =  %s \n", wstr2str(str).c_str());

		if (!MotionSetSymbol)
			dbg::Log("error occur, MotionSetSymbol is NULL \n");

		return true;
	}
	else
	{
		dbg::Log("error dll load %s, errorCode=0x%x\n", dllFileName.c_str(), GetLastError());
	}

	return false;
}