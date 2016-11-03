#include "stdafx.h"
#include "NoLimit2.h"
#include "NoLimit2Input.h"

using namespace motion;
using namespace std;


bool CustomModuleCallback(const motion::sComponent *parentComp, const string *protocol,
	const string *init, const string *math, const string *modulation, const vector<string*> &fomats);


cNoLimits2::cNoLimits2()
	: m_state(OFF)
	, m_delaySeconds(0)
	, m_lastSpeed(0)
	, m_speedZeroCount(0)
	, m_speedUpCount(0)
	, m_hWnd(NULL)
	, m_startTime(0)
	, m_gameIdx(0)
{
}

cNoLimits2::~cNoLimits2()
{
}


int cNoLimits2::Init(HWND hWnd, int gameIdx)
{
	m_state = SERVOON;
	m_hWnd = hWnd;
	m_gameIdx = gameIdx;
	m_startTime = 0;

	motion::cController2::Get()->CreateCustomModuleCallback(CustomModuleCallback);
	return motion::cController2::Get()->Init(hWnd, "Plugins/nolimits2.txt");
}


int cNoLimits2::UpdateMotionScript()
{
	return motion::cController2::Get()->Reload("Plugins/nolimits2.txt");
}


// return value 0: when servo off
//					    1: play game
//						2: game end
int cNoLimits2::Update(const float deltaSeconds)
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
		// send motion board by serial communication
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
		reVal = 2;
		Delay(3, READY);
		break;

	case STOP:
		cController2::Get()->SetOutputFormat(0, PRT_STOP);
		SendSerialPort();
		Delay(0, SERVOOFF);
		break;

	case SERVOOFF:
//		cController2::Get()->SetOutputFormat(0, PRT_SERVOOFF);
// 		SendSerialPort();
// 		Sleep(1000);
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


void cNoLimits2::End()
{
	m_state = STOP;
}


void cNoLimits2::Clear()
{
	m_state = OFF;
	m_delaySeconds = 0;
	m_speedUpCount = 0;
	m_speedZeroCount = 0;
}


void cNoLimits2::Delay(const float delaySeconds, STATE nextState)
{
	m_delaySeconds = delaySeconds;
	m_nextState = nextState;
	m_state = DELAY;
}


void cNoLimits2::SendSerialPort()
{
	for (int i = 0; i < 5; ++i)
	{
		cController2::Get()->Update(0.1f); // Send Serial Port
		Sleep(100);
	}
}


void cNoLimits2::CheckGameStart()
{
	const int gamestate = (int)script::g_symbols["@gamestate"].fVal;
	const float speed = script::g_symbols["@speed"].fVal;

 	if ((gamestate == 3) && (speed > 0))
 	{
		++m_speedUpCount;
		if (m_speedUpCount > 5)
		{
			// start motion
			m_state = START;
			m_speedUpCount = 0;
			m_speedZeroCount = 0;

			if (0 == m_startTime)
				m_startTime = timeGetTime();
		}
 	}

	m_lastSpeed = speed;
}


void cNoLimits2::CheckGameStop()
{
	const int gamestate = (int)script::g_symbols["@gamestate"].fVal;
	const float speed = script::g_symbols["@speed"].fVal;
	const float lapTime = script::g_symbols["@time"].fVal;
 
	if (gamestate == 0)
	{
		m_state = TOREADY; // when game end, motion stop

		m_startTime = 0; // initialize when game end
		m_speedUpCount = 0;
		m_speedZeroCount = 0;
	}
	else if (speed == 0)
	{
		++m_speedZeroCount;
		if ((m_speedZeroCount > 150) && (lapTime > 6000)) // about 3~4 seconds, lapTime > 60 seconds
		{
			m_state = TOREADY; // when game end, motion stop

			// press ESC key
			KeyDown(VK_ESCAPE);

 			m_startTime = 0; // initialize when game end
			m_speedUpCount = 0;
			m_speedZeroCount = 0;
			cController2::Get()->WriteGameResult("NoLimits2", "UserID", "track name", lapTime, 0, 0);
		}
 	}
}


bool cNoLimits2::KeyDown(const int vkey)
{
	if (vkey <= 0)
		return false;

	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey((WORD)vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = (WORD)vkey;
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(30);

	input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(30);

	return true;
}


bool CustomModuleCallback(const motion::sComponent *parentComp, const string *protocol, const string *init,
	const string *math, const string *modulation, const vector<string*> &fomats)
{
	string ip = "127.0.0.1";
	int port = 15151;
	if (parentComp->cmd)
	{
		ip = parentComp->cmd->values["ip"];
		port = atoi(parentComp->cmd->values["port"].c_str());
	}

	cNoLimits2Input *input = new cNoLimits2Input();
	if (input->Init(ip, port, init, math, modulation))
	{
		motion::cController2::Get()->AddInput(input);
		return true;
	}

	return false;
}

