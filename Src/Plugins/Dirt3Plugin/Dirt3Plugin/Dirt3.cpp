
#include "stdafx.h"
#include "Dirt3.h"

using namespace motion;
using namespace std;


cDirt3::cDirt3()
	: m_state(OFF)
	, m_delaySeconds(0)
	, m_lastLapTime(0)
	, m_lapTime(0)
	, m_lapTimeUpCount(0)
	, m_sameLapTimeCount(0)
	, m_hWnd(NULL)
{
}

cDirt3::~cDirt3()
{
}


int cDirt3::Init(HWND hWnd)
{
	m_state = SERVOON;
	m_hWnd = hWnd;
	return motion::cController2::Get()->Init(hWnd, "Plugins/dirt3.txt");
}


int cDirt3::UpdateMotionScript()
{
	return motion::cController2::Get()->Reload("Plugins/dirt3.txt");
}


int cDirt3::Update(const float deltaSeconds)
{
	int reVal = 1; // return 0, when servo off

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
		CheckGameStart();
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
		CheckGameStop();
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
	m_lastLapTime = 0;
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


void cDirt3::CheckGameStart()
{
 	const float lapTime = script::g_symbols["@laptime"].fVal;
	if (lapTime > m_lastLapTime)
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

	m_lastLapTime = lapTime;
}


void cDirt3::CheckGameStop()
{
	const float lapTime = script::g_symbols["@laptime"].fVal;
	const float distance = script::g_symbols["@distance"].fVal;
	if ((lapTime == m_lastLapTime) && (m_lastLapTime == 0) && (distance != 0))
	{
		// when game end, motion stop
		m_state = TOREADY;

		cController2::Get()->WriteGameResult("Dirt3", "UserID", "track name", m_lapTime, 0, 0);
		cController2::Get()->WriteGameResultToDB("Dirt3", "UserID", "track name", m_lapTime, 0, 0);
	}
	else if (lapTime == m_lastLapTime) // when show pause menu
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

	m_lastLapTime = lapTime;
	if (0 != lapTime)
		m_lapTime = lapTime;
}

