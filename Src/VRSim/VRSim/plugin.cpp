
#include "stdafx.h"
#include "plugin.h"


bool sPluginInfo::Load(const string &dllFileName)
{
	lib = LoadLibraryA(dllFileName.c_str());
	if (lib)
	{
		GetGameName = (__GetGameName)GetProcAddress(lib, "GetGameName");
		GetOutputGameName = (__GetOutputGameName)GetProcAddress(lib, "GetOutputGameName");
		MotionInit = (__MotionInit)GetProcAddress(lib, "MotionInit");
		MotionUpdateScript = (__MotionUpdateScript)GetProcAddress(lib, "MotionUpdateScript");
		MotionUpdate = (__MotionUpdate)GetProcAddress(lib, "MotionUpdate");
		MotionEnd = (__MotionEnd)GetProcAddress(lib, "MotionEnd");
		MotionClear = (__MotionClear)GetProcAddress(lib, "MotionClear");

		if (GetGameName)
			GetGameName(gameName);
		if (GetOutputGameName)
			GetOutputGameName(outputGameName);
		
		dbg::Log("dll load %s \n", dllFileName.c_str());
		return true;
	}
	else
	{
		dbg::Log("error dll load %s, errorCode=0x%x\n", dllFileName.c_str(), GetLastError());
	}

	return false;
}