
#include "stdafx.h"
#include "WarThunder.h"
#include "WarThunderInput.h"

using namespace std;
using namespace motion;

bool CustomModuleCallback(const motion::sComponent *parentComp, const string *protocol,
	const string *init, const string *math, const string *modulation, const vector<string*> &fomats);


cWarThunder::cWarThunder()
	: m_state(OFF)
	, m_delaySeconds(0)
	, m_validCount(0)
	, m_hWnd(NULL)
{
}

cWarThunder::~cWarThunder()
{
}


int cWarThunder::Init(HWND hWnd)
{
	m_state = SERVOON;
	m_hWnd = hWnd;

	motion::cController2::Get()->CreateCustomModuleCallback(CustomModuleCallback);
	return motion::cController2::Get()->Init(hWnd, "Plugins/warthunder.txt");
}


int cWarThunder::UpdateMotionScript()
{
	return motion::cController2::Get()->Reload("Plugins/warthunder.txt");
}


int cWarThunder::Update(const float deltaSeconds)
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


void cWarThunder::End()
{
	m_state = STOP;
}


void cWarThunder::Clear()
{
	m_state = OFF;
	m_delaySeconds = 0;
	m_validCount = 0;
}


void cWarThunder::Delay(const float delaySeconds, STATE nextState)
{
	m_delaySeconds = delaySeconds;
	m_nextState = nextState;
	m_state = DELAY;
}


void cWarThunder::SendSerialPort()
{
	for (int i = 0; i < 5; ++i)
	{
		cController2::Get()->Update(0.1f); // Send Serial Port
		Sleep(100);
	}
}


void cWarThunder::CheckGameStart()
{
	const float valid = script::g_symbols["@indicator_valid"].fVal;
	if (valid > 0)
	{
		++m_validCount;
		if (m_validCount > 5)
		{
			// start motion
			m_state = START;
			m_validCount = 0;
		}
	}
}


void cWarThunder::CheckGameStop()
{
	const float valid = script::g_symbols["@indicator_valid"].fVal;
	if (valid <= 0.f)
	{
		++m_validCount;
		if (m_validCount > 20)
		{
			m_state = TOREADY;
		}
	}
	else
	{
		m_validCount = 0;
	}
}



bool CustomModuleCallback(const motion::sComponent *parentComp, const string *protocol, const string *init, 
	const string *math, const string *modulation, const vector<string*> &fomats)
{
	cWarThunderInput *input = new cWarThunderInput();
	if (input->Init(init, math, modulation))
	{
		motion::cController2::Get()->AddInput(input);
		return true;
	}

	return false;
}
