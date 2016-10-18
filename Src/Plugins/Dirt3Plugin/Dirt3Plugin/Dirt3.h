//
// 2016-07-11, jjuiddong
// Dirt3 Motion
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

	int Init(HWND hWnd);
	int UpdateMotionScript();
	int Update(const float deltaSeconds);
	void End();
	void Clear();


protected:
	void Delay(const float delaySeconds, STATE nextState);
	void SendSerialPort();
	void CheckGameStart();
	void CheckGameStop();


protected:
	STATE m_state;
	HWND m_hWnd;

	// delay
	STATE m_nextState;
	float m_delaySeconds;

	// labTime
	float m_lastLapTime;
	float m_lapTime;
	int m_lapTimeUpCount;
	int m_sameLapTimeCount;
};

