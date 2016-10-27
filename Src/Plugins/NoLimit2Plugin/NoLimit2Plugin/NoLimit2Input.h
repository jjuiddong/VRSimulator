//
// Get the NoLimits2 Motion data from TCP Protocol
//
#pragma once

#include <thread>


class cNoLimits2Input : public motion::cInput
{
public:
	cNoLimits2Input();
	virtual ~cNoLimits2Input();

	bool Init(const string &ip, const int port, const string *init, const string *math, const string *modulation);
	virtual bool Start() override;
	virtual bool Stop() override;
	virtual bool Update(const float deltaSeconds) override;


public:
	string m_ip;
	int m_port;

	boost::interprocess::windows_shared_memory m_sharedmem;
	boost::interprocess::mapped_region m_mmap;
	BYTE *m_memPtr;

	// thread variable
	std::thread m_thread;
	bool m_loop;
	int m_sleepMillis;
	common::CriticalSection m_critcalSection;
};

