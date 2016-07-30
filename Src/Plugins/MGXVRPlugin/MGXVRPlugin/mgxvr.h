//
// 2016-07-14, jjuiddong
// 머신건X VR 모션 처리
//
#pragma once

class cMGXVR
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

	cMGXVR();
	virtual ~cMGXVR();

	int Init(HWND hWnd);
	int UpdateMotionScript();
	int Update(const float deltaSeconds);
	void End();
	void Clear();


protected:
	void Delay(const float delaySeconds, STATE nextState);
	void SendSerialPort();
	void CheckAutoStart();
	void CheckAutoStop();


protected:
	STATE m_state;
	HWND m_hWnd;

	// delay
	STATE m_nextState;
	float m_delaySeconds;

	// labTime
	float m_lastLabTime;
	int m_lapTimeUpCount;
	int m_sameLapTimeCount;
};

