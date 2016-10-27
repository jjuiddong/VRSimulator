//
// 2016-07-11, jjuiddong
// Dirt3 Motion
//
// 2016-10-21
//		- time up state
//
#pragma once

class cDirt3
{
public:
	enum STATE {
		OFF,
		SERVOON,
		READY,
		START,
		PLAY,
		TOREADY,
		TIMEUP_STOP,
		TIMEUP_TOREADY,
		STOP,
		SERVOOFF,
		DELAY,
	};

	enum MOTION_PROTOCOL {
		PRT_MOTION,
		PRT_SERVOON,
		PRT_SERVOOFF,
		PRT_START,
		PRT_STOP,
	};

	cDirt3();
	virtual ~cDirt3();

	int Init(HWND hWnd, int gameIdx);
	int UpdateMotionScript();
	int Update(const float deltaSeconds);
	void End();
	void Clear();


protected:
	void Delay(const float delaySeconds, STATE nextState);
	void SendSerialPort();
	void CheckGameStart();
	void CheckGameStop();
	void CheckGameFinish();


protected:
	STATE m_state;
	HWND m_hWnd;
	int m_gameIdx;

	// delay
	STATE m_nextState;
	float m_delaySeconds;

	// lapTime
	int m_startTime;
	float m_lastLapTime;
	float m_lapTime;
	int m_lapTimeUpCount;
	int m_sameLapTimeCount;
};

