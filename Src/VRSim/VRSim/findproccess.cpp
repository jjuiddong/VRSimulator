
#include "stdafx.h"
#include "findproccess.h"
#include 	"TlHelp32.h"


void FindProcessThreadFunc(cFindProcess *findProc);


cFindProcess::cFindProcess()
	: m_loop(true)
{
	m_processes.reserve(128);
}

cFindProcess::~cFindProcess()
{
}


// start find process
bool cFindProcess::Init()
{
	if (m_thread.joinable())
	{
		m_loop = false;
		m_thread.join();
	}

	m_loop = true;
	m_thread = std::thread(FindProcessThreadFunc, this);
	return true;
}


// terminate thread, and clear all variable
void cFindProcess::Clear()
{
	if (m_thread.joinable())
	{
		m_loop = false;
		m_thread.join();
	}

	m_runningProcess.clear();
	m_processes.clear();
}


bool cFindProcess::AddFindProcess(const wstring &processName)
{
	AutoCSLock cs(m_csList);
	m_processes.push_back(processName);
	return true;
}


wstring cFindProcess::GetRunningProcess()
{
	AutoCSLock cs(m_csRunProc);
	if (m_runningProcess.empty())
		return L"";
	wstring reval = m_runningProcess;
	return reval;
}


void FindProcessThreadFunc(cFindProcess *findProc)
{
	const bool isFindWindowName = false; // default:false, debugging vlaue

	while (findProc->m_loop)
	{
		AutoCSLock cs(findProc->m_csList);

		wstring runningProcess;

		//-----------------------------------------------------------------------------
		// debugging code
		if (isFindWindowName)
		{
			for each (auto &str in findProc->m_processes)
			{
				if (FindWindow(NULL, str.c_str()))
				{
					runningProcess = str;
					break;
				}
			}
			if (runningProcess != findProc->m_runningProcess)
			{
				AutoCSLock cs2(findProc->m_csRunProc);
				findProc->m_runningProcess = runningProcess;
			}
			if (findProc->m_loop)
				std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // 2 seconds sleep
			continue;
		}
		//-----------------------------------------------------------------------------


		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) 
		{
			// nothing~
		}
		else 
		{
			PROCESSENTRY32 pe32;
			pe32.dwSize = sizeof(PROCESSENTRY32);
			if (Process32First(hProcessSnap, &pe32)) 
			{
				auto it = std::find(findProc->m_processes.begin(), findProc->m_processes.end(), pe32.szExeFile);
				if (it != findProc->m_processes.end()) 
				{
					// find process
					runningProcess = pe32.szExeFile;
				}
				else 
				{
					while (Process32Next(hProcessSnap, &pe32) && findProc->m_loop)
					{
						it = std::find(findProc->m_processes.begin(), findProc->m_processes.end(), pe32.szExeFile);
						if (it != findProc->m_processes.end())
						{
							// find process
							runningProcess = pe32.szExeFile;
							break;
						}
					}
				}

				if (runningProcess != findProc->m_runningProcess)
				{
					AutoCSLock cs2(findProc->m_csRunProc);
					findProc->m_runningProcess = runningProcess;
				}

				CloseHandle(hProcessSnap);
			}
		}

		if (findProc->m_loop)
			std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // 2 seconds sleep
	} // end while

}
