
#include "global.h"
#include "warthundermotioninput.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace motion;
using namespace std;
using namespace common;

unsigned WINAPI WarThunderThreadFunc(void* arg);


cWarThunderMotionInput::cWarThunderMotionInput()
	: m_handle(NULL)
	, m_sleepMillis(50)
	, m_threadLoop(true)
	, m_inputData(INDICATORS)
	, m_Roll(0)
	, m_Pitch(0)
	, m_Yaw(0)
{
}

cWarThunderMotionInput::~cWarThunderMotionInput()
{
	Clear();
}


bool cWarThunderMotionInput::Init(const MOTION_DATA inputData)
{
	m_inputData = inputData;
	m_handle = (HANDLE)_beginthreadex(NULL, 0, WarThunderThreadFunc, this, 0, (unsigned*)&m_threadId);
	return true;
}


void cWarThunderMotionInput::GetMotion(OUT float &roll, OUT float &pitch, OUT float &yaw)
{
	AutoCSLock cs(m_critcalSection);
	roll = m_Roll;
	pitch = m_Pitch;
	yaw = m_Yaw;
}


void cWarThunderMotionInput::Clear()
{
	m_threadLoop = false;
	if (m_handle)
	{
		::WaitForSingleObject(m_handle, 1000);
		m_handle = NULL;
	}
}


// 주기적으로, War Thunder Motion 정보를 가져온다.
unsigned WINAPI WarThunderThreadFunc(void* arg)
{
	cWarThunderMotionInput *wtInput = (cWarThunderMotionInput*)arg;
	network::cTCPClient client;

	cWarThunderMotionInput::MOTION_DATA readType = cWarThunderMotionInput::STATE;
	string cmd[] = {
		"GET /state HTTP/1.1\r\n \r\nConnection: close\r\n\r\n",
		"GET /indicators HTTP/1.1\r\n \r\nConnection: close\r\n\r\n"
	};

	while (wtInput->m_threadLoop)
	{
		if (!client.Init("localhost", 8111, 512, 10, 30, true))
		{
			Sleep(100);
			continue;
		}

		if ((wtInput->m_inputData != readType) && (wtInput->m_inputData != cWarThunderMotionInput::ALL))
			readType = (readType == cWarThunderMotionInput::STATE) ? cWarThunderMotionInput::INDICATORS : cWarThunderMotionInput::STATE;

		const string str = cmd[readType];
		client.Send((BYTE*)str.c_str(), str.length());

		// Receive Motion Data from server
		stringstream ss;
		while (client.IsConnect())
		{
			network::sSockBuffer packet;
			while (client.m_recvQueue.Front(packet))
			{
				packet.buffer[packet.actualLen] = NULL;
				ss << packet.buffer;
				client.m_recvQueue.Pop();
			}
		}

		// json Parsing
		try
		{
			using boost::property_tree::ptree;
			using std::string;
			ptree props;

			const string str = ss.str();
			const char *next = strchr(str.c_str(), '{');
			if (next)
			{
				std::istringstream iss(next);
				boost::property_tree::read_json(iss, props);

				switch (readType)
				{
				case cWarThunderMotionInput::STATE:
					break;

				case cWarThunderMotionInput::INDICATORS:
				{
					string roll = props.get<string>("aviahorizon_roll", "");
					string pitch = props.get<string>("aviahorizon_pitch", "");
					string bank = props.get<string>("bank", "");
					string compass = props.get<string>("compass", "");
					string compass1 = props.get<string>("compass1", "");

					wtInput->m_critcalSection.Lock();
					wtInput->m_Roll = (float)atof(roll.c_str());
					wtInput->m_Pitch = (float)atof(pitch.c_str());
					wtInput->m_Yaw = ANGLE2RAD((float)atof(compass.c_str()));
					wtInput->m_Bank = (float)atof(bank.c_str()); 
					wtInput->m_critcalSection.Unlock();

					//cout << "roll= " << roll << ", pitch=" << pitch << ", bank= " << bank << ", compass= " << compass << ", compass1= " << compass1<< endl;
				}
				break;
				}
			}
		}
		catch (...)
		{
			// nothing~
		}

		// next read motion data
		readType = (readType == cWarThunderMotionInput::STATE) ? cWarThunderMotionInput::INDICATORS : cWarThunderMotionInput::STATE;

		Sleep(wtInput->m_sleepMillis);
	}

	return 0;
}

