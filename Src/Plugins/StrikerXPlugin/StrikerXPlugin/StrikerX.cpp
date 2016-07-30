
#include "stdafx.h"
#include "StrikerX.h"

using namespace motion;
using namespace std;


cStrikerX::cStrikerX()
	: m_state(OFF)
	, m_delaySeconds(0)
	, m_lastLabTime(0)
	, m_lapTimeUpCount(0)
	, m_sameLapTimeCount(0)
	, m_hWnd(NULL)
	, m_oldGameState(false)
	, m_oldRoll(0)
	, m_oldPitch(0)
	, m_oldYaw(0)
	, m_oldHeave(0)
	, m_oldTime(0)
	, m_sameDataCount(0)
	, m_checkDiffTime(0)
{
}

cStrikerX::~cStrikerX()
{
}


int cStrikerX::Init(HWND hWnd)
{
	m_state = SERVOON;
	m_hWnd = hWnd;
	return motion::cController2::Get()->Init(hWnd, "Plugins/strikerx.txt");
}


int cStrikerX::UpdateMotionScript()
{
	return motion::cController2::Get()->Reload("Plugins/strikerx.txt");
}


int cStrikerX::Update(const float deltaSeconds)
{
	int reVal = 1; // Servo �� Off �� ��, 0���� �����ȴ�.

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
		CheckAutoStart(deltaSeconds);
		break;

	case START:
	{
		cController2::Get()->SetOutputFormat(0, PRT_START);
		SendSerialPort();
		Sleep(500); // Delay
		cController2::Get()->SetOutputFormat(0, PRT_MOTION);
		SendSerialPort();
		Delay(0, PLAY);

		m_sameDataCount = 0;
		m_differentDataCount = 0;
		script::g_symbols["@shoot_enable"].type = script::FIELD_TYPE::T_FLOAT;
		script::g_symbols["@shoot_enable"].fVal = 0;
	}
	break;

	case PLAY:
	{
		const bool gameState = script::g_symbols["@gamestate"].fVal > 0 ? true : false;
		if (m_oldGameState != gameState)
		{
			// gamestate �� 1�� �Ǹ�, ����� �����Ѵ�.
			// gamestate �� 0�� �Ǹ�, ����� �����Ѵ�.
			if (gameState)
				script::g_symbols["@shoot_enable"].fVal = 1;
			else
				script::g_symbols["@shoot_enable"].fVal = 0;

			m_oldGameState = gameState;
		}

		CheckAutoStop(deltaSeconds);
	}
	break;

	case TOREADY:
		cController2::Get()->SetOutputFormat(0, PRT_STOP);
		SendSerialPort();
		Delay(3, READY);

		m_sameDataCount = 0;
		m_differentDataCount = 0;
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


void cStrikerX::End()
{
	m_state = STOP;
}


void cStrikerX::Clear()
{
	m_state = OFF;
	m_delaySeconds = 0;
	m_lastLabTime = 0;
	m_lapTimeUpCount = 0;
	m_sameLapTimeCount = 0;
}


void cStrikerX::Delay(const float delaySeconds, STATE nextState)
{
	m_delaySeconds = delaySeconds;
	m_nextState = nextState;
	m_state = DELAY;
}


void cStrikerX::SendSerialPort()
{
	for (int i = 0; i < 5; ++i)
	{
		cController2::Get()->Update(0.1f); // Send Serial Port
		Sleep(100);
	}
}


void cStrikerX::CheckAutoStart(const float deltaSeconds)
{
	// 1�ʿ� 10�� ����Ѵ�.
	m_checkDiffTime += deltaSeconds;
	if (m_checkDiffTime < 0.1f)
		return;
	m_checkDiffTime = 0;

	// 1�ʿ� 10�� ����Ѵ�.
	const bool gameState = script::g_symbols["@gamestate"].fVal > 0 ? true : false;
	const float roll = script::g_symbols["@roll"].fVal;
	const float pitch = script::g_symbols["@pitch"].fVal;
	const float yaw = script::g_symbols["@yaw"].fVal;
	const float heave = script::g_symbols["@heave"].fVal;
	const float timeSeconds = script::g_symbols["@time"].fVal;

	if (gameState)
	{
		// 0.5�� ���� �������� ��ȭ�� �ִٸ�, ������ ���۵� ������ �Ǵ��Ѵ�.
		if ((abs(roll - m_oldRoll) > 0.001f) ||
			(abs(pitch - m_oldPitch) > 0.001f) ||
			(abs(yaw - m_oldYaw) > 0.001f) ||
			(abs(heave - m_oldHeave) > 0.001f) ||
			(abs(timeSeconds - m_oldTime) > 0.001f)
			)
		{
			++m_differentDataCount;

			// �ٸ� ������ ������ ī��Ʈ�� �����Ѵ�. ���� ������ ������, 0���� �ʱ�ȭ �Ѵ�.
			if (m_differentDataCount > 5)
			{
				//m_vitconSim.Play();
				m_state = START;
				m_differentDataCount = 0;
				script::g_symbols["@shoot_enable"].fVal = 1;
			}
		}
		else
		{
			m_differentDataCount = 0;
		}
	}
	else
	{
		m_differentDataCount = 0;
	}

	m_oldRoll = roll;
	m_oldPitch = pitch;
	m_oldYaw = yaw;
	m_oldHeave = heave;
	m_oldTime = timeSeconds;
}


void cStrikerX::CheckAutoStop(const float deltaSeconds)
{
	// 1�ʿ� 10�� ����Ѵ�.
	m_checkDiffTime += deltaSeconds;
	if (m_checkDiffTime < 0.1f)
		return;
	m_checkDiffTime = 0;

	const bool gameState = script::g_symbols["@gamestate"].fVal > 0 ? true : false;
	const float roll = script::g_symbols["@roll"].fVal;
	const float pitch = script::g_symbols["@pitch"].fVal;
	const float yaw = script::g_symbols["@yaw"].fVal;
	const float heave = script::g_symbols["@heave"].fVal;
	const float timeSeconds = script::g_symbols["@time"].fVal;

	// ��� ���� Ȯ��
	// 1�� ���� �������� ��ȭ�� ������, ������ ����� ������ �Ǵ��Ѵ�.
	if ((abs(roll - m_oldRoll) < 0.00001f) &&
		(abs(pitch - m_oldPitch) < 0.00001f) &&
		(abs(yaw - m_oldYaw) < 0.00001f) &&
		(abs(heave - m_oldHeave) < 0.00001f) &&
		(abs(timeSeconds - m_oldTime) < 0.00001f))
	{
		++m_sameDataCount;

		// ���� ������ ���� �ð� �̻� �´ٸ�.. ����
		if (m_sameDataCount > 5)
		{
			//m_vitconSim.Stop();
			m_state = TOREADY;
			m_sameDataCount = 0;
			script::g_symbols["@shoot_enable"].fVal = 0;
		}
	}
	else
	{
		m_sameDataCount = 0;
	}

	m_oldRoll = roll;
	m_oldPitch = pitch;
	m_oldYaw = yaw;
	m_oldHeave = heave;
	m_oldTime = timeSeconds;
}
