
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
		MotionUpdate = (__MotionUpdate)GetProcAddress(lib, "MotionUpdate");
		MotionEnd = (__MotionEnd)GetProcAddress(lib, "MotionEnd");
		MotionClear = (__MotionClear)GetProcAddress(lib, "MotionClear");

		if (GetGameName)
			GetGameName(gameName);
		if (GetOutputGameName)
			GetOutputGameName(outputGameName);

		return true;
	}

	return false;
}