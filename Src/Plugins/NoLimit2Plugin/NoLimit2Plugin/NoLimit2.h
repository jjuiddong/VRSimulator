//
// 2016-10-26, jjuiddong
// NoLimits2 Motion
//
#pragma once

class cNoLimits2
{
public:
	enum STATE {
		OFF,
		SERVOON,
		READY,
		START,
		PLAY,
		TOREADY,
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

	cNoLimits2();
	virtual ~cNoLimits2();

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
	bool KeyDown(const int vkey);


protected:
	STATE m_state;
	HWND m_hWnd;
	int m_gameIdx;

	// delay
	STATE m_nextState;
	float m_delaySeconds;

	// lapTime
	int m_startTime;
	float m_lastSpeed;
	int m_speedUpCount;
	int m_speedZeroCount;
};

