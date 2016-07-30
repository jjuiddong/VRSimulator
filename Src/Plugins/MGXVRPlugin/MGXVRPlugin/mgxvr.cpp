
#include "stdafx.h"
#include "mgxvr.h"

using namespace motion;
using namespace std;


cMGXVR::cMGXVR()
	: m_state(OFF)
	, m_delaySeconds(0)
	, m_lastLabTime(0)
	, m_lapTimeUpCount(0)
	, m_sameLapTimeCount(0)
	, m_hWnd(NULL)
{
}

cMGXVR::~cMGXVR()
{
}


int cMGXVR::Init(HWND hWnd)
{
	m_state = SERVOON;
	m_hWnd = hWnd;
	return motion::cController2::Get()->Init(hWnd, "Plugins/mgxvr.txt");
}


int cMGXVR::UpdateMotionScript()
{
	return motion::cController2::Get()->Reload("Plugins/mgxvr.txt");
}


int cMGXVR::Update(const float deltaSeconds)
{
	int reVal = 1; // Servo 가 Off 될 때, 0으로 설정된다.

	switch (m_state)
	{
	case OFF:
		reVal = 0;
		break;

	case SERVOON:
	{
		cController2::Get()->Start();
		cController2::Get()->SetOutputFormat(0, PRT_SERVOON);
		SendSerialPort();
		Delay(1, READY);
	}
	break;

	case READY:
		CheckAutoStart();
		break;

	case START:
	{
		cController2::Get()->SetOutputFormat(0, PRT_START);
		SendSerialPort();
		Sleep(500); // Delay
		cController2::Get()->SetOutputFormat(0, PRT_MOTION);
		SendSerialPort();
		Delay(0, PLAY);
	}
	break;

	case PLAY:
		CheckAutoStop();
		break;

	case TOREADY:
		cController2::Get()->SetOutputFormat(0, PRT_STOP);
		SendSerialPort();
		Delay(3, READY);
		break;

	case STOP:
		cController2::Get()->SetOutputFormat(0, PRT_STOP);
		SendSerialPort();
		Delay(3, SERVOOFF);
		break;

	case SERVOOFF:
		cController2::Get()->SetOutputFormat(0, PRT_SERVOOFF);
		SendSerialPort();
		Sleep(1000);
		cController2::Get()->Stop();
		Delay(0, OFF);
		break;

	case DELAY:
	{
		m_delaySeconds -= deltaSeconds;
		if (m_delaySeconds < 0)
			m_state = m_nextState;
	}
	break;

	default:
		break;
	}

	cController2::Get()->Update(deltaSeconds);

	return reVal;
}


void cMGXVR::End()
{
	m_state = STOP;
}


void cMGXVR::Clear()
{
	m_state = OFF;
	m_delaySeconds = 0;
	m_lastLabTime = 0;
	m_lapTimeUpCount = 0;
	m_sameLapTimeCount = 0;
}


void cMGXVR::Delay(const float delaySeconds, STATE nextState)
{
	m_delaySeconds = delaySeconds;
	m_nextState = nextState;
	m_state = DELAY;
}


void cMGXVR::SendSerialPort()
{
	for (int i = 0; i < 5; ++i)
	{
		cController2::Get()->Update(0.1f); // Send Serial Port
		Sleep(100);
	}
}


void cMGXVR::CheckAutoStart()
{
	const float lapTime = script::g_symbols["@laptime"].fVal;
	if (lapTime > m_lastLabTime)
	{
		++m_lapTimeUpCount;
		if (m_lapTimeUpCount > 5)
		{
			// start motion
			m_state = START;
			m_lapTimeUpCount = 0;
			m_sameLapTimeCount = 0;
		}
	}

	m_lastLabTime = lapTime;
}


void cMGXVR::CheckAutoStop()
{
	const float lapTime = script::g_symbols["@laptime"].fVal;
	const float distance = script::g_symbols["@distance"].fVal;
	if ((lapTime == m_lastLabTime) && (m_lastLabTime == 0) && (distance != 0))
	{
		// motion stop
		m_state = TOREADY;
	}
	else if (lapTime == m_lastLabTime)
	{
		++m_sameLapTimeCount;
		if (m_sameLapTimeCount > 20)
		{
			m_state = TOREADY;
		}
	}
	else
	{
		m_sameLapTimeCount = 0;
	}

	m_lastLabTime = lapTime;
}

