//
// 2016-09-24, jjuiddong
// ProjectCars Motion
//
#pragma once

class cProjectCars
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

	cProjectCars();
	virtual ~cProjectCars();

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
	int m_delayCount;
};

