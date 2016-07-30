
#include "stdafx.h"
#include "Dirt3.h"

using namespace motion;


cDirt3::cDirt3()
	: m_state(OFF)
	, m_delaySeconds(0)
	, m_lastLabTime(0)
	, m_lapTimeUpCount(0)
	, m_sameLapTimeCount(0)
{
}

cDirt3::~cDirt3()
{
}


int cDirt3::Init(HWND hWnd)
{
	m_state = SERVOON;
	return motion::cController2::Get()->Init(hWnd, "dirt3.txt");
}


int cDirt3::Update(const float deltaSeconds)
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


void cDirt3::End()
{
	m_state = STOP;
}


void cDirt3::Clear()
{
	m_state = OFF;
	m_delaySeconds = 0;
	m_lastLabTime = 0;
	m_lapTimeUpCount = 0;
	m_sameLapTimeCount = 0;
}


void cDirt3::Delay(const float delaySeconds, STATE nextState)
{
	m_delaySeconds = delaySeconds;
	m_nextState = nextState;
	m_state = DELAY;
}


void cDirt3::SendSerialPort()
{
	for (int i = 0; i < 5; ++i)
	{
		cController2::Get()->Update(0.1f); // Send Serial Port
		Sleep(100);
	}
}


void cDirt3::CheckAutoStart()
{
	float lapTime = script::g_symbols["@laptime"].fVal;
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


void cDirt3::CheckAutoStop()
{
	float lapTime = script::g_symbols["@laptime"].fVal;
	float distance = script::g_symbols["@distance"].fVal;
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

