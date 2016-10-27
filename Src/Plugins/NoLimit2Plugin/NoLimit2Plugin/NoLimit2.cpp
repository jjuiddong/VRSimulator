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
	, m_lastLapTime(0)
	, m_lapTime(0)
	, m_lapTimeUpCount(0)
	, m_sameLapTimeCount(0)
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

	case TIMEUP_STOP:
		cController2::Get()->SetOutputFormat(0, PRT_STOP);
		SendSerialPort();
		reVal = 2;
		Delay(3, TIMEUP_TOREADY);
		break;

	case TIMEUP_TOREADY:
		CheckGameFinish();
		break;

	case STOP:
		cController2::Get()->SetOutputFormat(0, PRT_STOP);
		SendSerialPort();
		Delay(3, SERVOOFF);
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
	m_lastLapTime = 0;
	m_lapTimeUpCount = 0;
	m_sameLapTimeCount = 0;
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

			if (0 == m_startTime)
				m_startTime = timeGetTime();
		}
	}

	m_lastLapTime = lapTime;
}


void cNoLimits2::CheckGameStop()
{
	const float lapTime = script::g_symbols["@laptime"].fVal;
	const float distance = script::g_symbols["@distance"].fVal;
	const float limitTime = script::g_symbols["@limit_time"].fVal;
	const float playTime = ((timeGetTime() - m_startTime) / 60000.f);
	const bool isTimeOver = playTime > limitTime;

	if (isTimeOver
		|| ((lapTime == m_lastLapTime) && (m_lastLapTime == 0) && (distance != 0)))
	{
		if (isTimeOver)
			m_state = TIMEUP_STOP; // when time over, motion stop
		else
			m_state = TOREADY; // when game end, motion stop

		m_startTime = 0; // initialize when game end

		string gameName[] = { "Dirt3", "GRID Autosport",  "DiRT Showdown" };
		cController2::Get()->WriteGameResult(gameName[m_gameIdx], "UserID", "track name", m_lapTime, 0, 0);

		const string dbIP = cController2::Get()->m_script.m_program->cmd->values["db_ip"];
		if (!dbIP.empty())
		{
			const int dbPort = atoi(cController2::Get()->m_script.m_program->cmd->values["db_port"].c_str());
			const string dbID = cController2::Get()->m_script.m_program->cmd->values["db_id"];
			const string dbPasswd = cController2::Get()->m_script.m_program->cmd->values["db_passwd"];
			const string databaseName = cController2::Get()->m_script.m_program->cmd->values["database_name"];
			cController2::Get()->WriteGameResultToDB(dbIP, dbPort, dbID, dbPasswd, databaseName,
				"Dirt3", "UserID", "track name", m_lapTime, 0, 0);
		}
	}
	else if (lapTime == m_lastLapTime) // when show pause menu, esc key or focus another window
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

	script::g_symbols["@play_time"].type = script::FIELD_TYPE::T_FLOAT;
	script::g_symbols["@play_time"].fVal = playTime;

	m_lastLapTime = lapTime;
	if (0 != lapTime)
		m_lapTime = lapTime;
}


void cNoLimits2::CheckGameFinish()
{
	if (script::g_symbols["@laptime"].fVal == 0.f)
	{
		m_state = READY;
		m_startTime = 0;
	}
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
