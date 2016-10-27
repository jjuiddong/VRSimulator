
#include "stdafx.h"
#include "NoLimit2Input.h"


using namespace motion;

unsigned WINAPI NoLimits2ThreadFunc(void* arg);


cNoLimits2Input::cNoLimits2Input()
	: motion::cInput(MOTION_MEDIA::TCP)
	, m_loop(true)
	, m_sleepMillis(1)
	, m_memPtr(NULL)
{
}

cNoLimits2Input::~cNoLimits2Input()
{
}


bool cNoLimits2Input::Init(const string &ip, const int port, 
	const string *init, const string *math, const string *modulation)
{
	using namespace boost::interprocess;

	m_ip = ip;
	m_port = port;

	try
	{
		m_sharedmem = windows_shared_memory(open_or_create, "nolimits2mem", read_write, 86);
		m_mmap = mapped_region(m_sharedmem, read_write, 0, 86);
		m_memPtr = static_cast<BYTE*>(m_mmap.get_address());
	}
	catch (interprocess_exception&e)
	{
		std::stringstream ss;
		ss << e.what() << std::endl;
		ss << "Shared Memory Error!!";
		dbg::ErrLog(ss.str().c_str());
		return false;
	}

	return true;
}


bool cNoLimits2Input::Start()
{
	if (m_thread.joinable())
	{
		m_loop = false;
		m_thread.join();
	}

	m_loop = true;
	m_thread = std::thread(NoLimits2ThreadFunc, this);

	m_state = MODULE_STATE::START;
	return true;
}


bool cNoLimits2Input::Stop()
{
	m_state = MODULE_STATE::STOP;

	if (m_thread.joinable())
	{
		m_loop = false;
		m_thread.join();
	}

	return true;
}


bool cNoLimits2Input::Update(const float deltaSeconds)
{
	using namespace script;

	RETV(m_state == MODULE_STATE::STOP, false);

	m_incTime += deltaSeconds;
	if (m_incTime < 0.033f)
		return true;

	m_incTime = 0;
	return true;
}


void SendTelemetry(network::cTCPClient &client)
{
	// Big Endian
	unsigned int n = 0;
	BYTE sbuffer[10];
	ZeroMemory(sbuffer, sizeof(sbuffer));
	sbuffer[0] = 'N';
	*(unsigned short*)(sbuffer + 2) = 5;
	*(unsigned int*)(sbuffer + 6) = n;
	*(unsigned short*)(sbuffer + 7) = 0;
	sbuffer[9] = 'L';
	client.Send(sbuffer, sizeof(sbuffer));
}


// get NoLimits2 Motion information
unsigned WINAPI NoLimits2ThreadFunc(void* arg)
{
	cNoLimits2Input *wtInput = (cNoLimits2Input*)arg;
	network::cTCPClient client;
	client.m_isLog = false;

	while (wtInput->m_loop)
	{
		if (client.Init(wtInput->m_ip, wtInput->m_port, 86, 10, 10, true))
			break;
	}

	SendTelemetry(client);

	int t0 = timeGetTime();
	int state = 0; // 0=recv, 1=send
	while (wtInput->m_loop)
	{
		if (state == 0)
		{
			network::sSockBuffer buffer;
			if (client.m_recvQueue.Front(buffer))
			{
				if (buffer.readLen < 86)
				{
					client.m_recvQueue.Pop();
					SendTelemetry(client);
					continue;
				}

				if (wtInput->m_memPtr)
					memcpy(wtInput->m_memPtr, buffer.buffer, 86);

				client.m_recvQueue.Pop();
				state = 1;
			}
		}
		else if (state == 1)
		{
			if (timeGetTime() - t0 > 30)
			{
				t0 = timeGetTime();
				SendTelemetry(client);
				state = 0;
			}
		}
	}

	return 0;
}

