//
// 2016-10-29, jjuiddong
// find running process with thread
//
#pragma once


class cFindProcess
{
public:
	cFindProcess();
	virtual ~cFindProcess();

	bool Init();
	void Clear();
	bool AddFindProcess(const wstring &processName);
	wstring GetRunningProcess();


public:
	bool m_loop;
	vector<wstring> m_processes;
	wstring m_runningProcess;

	std::thread m_thread;
	common::CriticalSection m_csList; // process list
	common::CriticalSection m_csRunProc; // running process
};
